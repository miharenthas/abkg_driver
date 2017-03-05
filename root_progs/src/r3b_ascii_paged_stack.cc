//implementation of the r3b_ascii_paged_stack cladd

#include "r3b_ascii_paged_stack.h"

//------------------------------------------------------------------------------------
//init the page size variable
unsigned int page_size = DEFAULT_PAGE_SIZE;

//------------------------------------------------------------------------------------
//ctors, dtor:

//------------------------------------------------------------------------------------
//default:
r3b_ascii_paged_stack::r3b_ascii_paged_stack():
	_own_page_sz( r3b_ascii_paged_stack::page_size ),
	_front_buf( new std::deque<r3b_ascii_event> ),
	_back_buf( new std::deque<r3b_ascii_event> ),
	_memory_sz( 0 )
{
	pthread_attr_init( &_op_attr );
	pthread_attr_setdetachstate( &_op_attr, PTHREAD_CREATE_JOINABLE );
	pthread_mutexattr_init( &_memory_sz_mutexattr );
};

//------------------------------------------------------------------------------------
//destructor:
r3b_ascii_paged_stack::~r3b_ascii_paged_stack(){
	delete _front_buf;
	delete _back_buf;
	
	//close the streams.
	//after killing the disk operator
	pthread_cancel( _disk_op_thread );
	while( !_pages.empty() ){ fclose( _pages.top() ); _pages.pop(); }
	
	//cleanup the attributes
	pthread_attr_destroy( &_op_attr );
	pthread_mutexattr_destroy( &_memory_sz_mutexattr );
}

//------------------------------------------------------------------------------------
//methods:

//------------------------------------------------------------------------------------
//push an element (and get the current size in memory)
unsigned int r3b_ascii_paged_stack::push( r3b_ascii_event &given ){
	_front_buf->push_back( given );
	
	//update the memory size
	pthread_mutex_lock( &_memory_sz_mutex );
	_memory_sz += sizeof(r3b_ascii_event)+given.nTracks*sizeof(r3b_ascii_track);
	pthread_mutex_unlock( &_memory_sz_mutex );
	
	if( _memory_sz > _own_page_sz ){
		swap_buffers(); //swap the buffers
	}
	
	return _front_buf->size();
}

//------------------------------------------------------------------------------------
//pop an element out of the container
r3b_ascii_event r3b_ascii_paged_stack::pop(){
	if( _front_buf->empty() ){
		swap_buffers();
	}
	
	r3b_ascii_event evt;
	if( !_front_buf->empty() ){
		evt = _front_buf->back();
		_front_buf->pop_back();
	} else { /*eventually, throw something;*/ }
	
	//update the memory size
	pthread_mutex_lock( &_memory_sz_mutex );
	_memory_sz -= sizeof(r3b_ascii_event)+evt.nTracks*sizeof(r3b_ascii_track);
	pthread_mutex_unlock( &_memory_sz_mutex );
	
	return evt;
}

//------------------------------------------------------------------------------------
//top: just look on top of the stack
r3b_ascii_event r3b_ascii_paged_stack::top(){
	if( _front_buf->empty() ) swap_buffers();
	if( !_front_buf->empty() ) return _front_buf->front();
	else{ r3b_ascii_event evt; return evt; }
}

//------------------------------------------------------------------------------------
//alloc and free for the a_page structure.
r3b_pstack::a_page *r3b_ascii_paged_stack::a_page_alloc( FILE *file,
                                                         std::deque<r3b_ascii_event> *bb ){
	r3b_ascii_paged_stack::a_page *f =
		(r3b_pstack::a_page*)malloc( sizeof(r3b_pstack::a_page) );
	f->file = file;
	f->bb = bb;
	f->caller = this;
	
	return f;
}

void r3b_ascii_paged_stack::a_page_free( r3b_pstack::a_page *a_file ){ free( a_file ); };

//------------------------------------------------------------------------------------
//the swap buffer method, and here we have a bit more complications
//NOTE: the expected usage of this stack is first a complete fillup
//      and then a complete emptying. So the structure of this function
//      works best with that kind of operations in mind.
void r3b_ascii_paged_stack::swap_buffers(){
	//always, begin with a join call
	//if no thread was instantiated or the one has terminated
	//this should have no consequences.
	pthread_join( _disk_op_thread, NULL );

	a_page *a_file;
	//when a buffer swap is requested, we have four cases:
	//1) the front buffer is empty and needs refilling, while the back buffer is full
	//2) the front buffer is full and needs emptying, while the back buffer is empty
	//3) the front buffer is empty AND the back buffer is empty
	//4) the front buffer is full AND the back buffer is full
	//the last two case aren't very fortunate, because they need a blocking call.
	if( _front_buf->empty() && !_back_buf->empty() ){
		//then the back buffer is full
		std::swap( _front_buf, _back_buf );
				
		//now, fill the back buffer:
		if( _pages.empty() ) return; //we have no files to load.
		a_file = a_page_alloc( _pages.top(), _back_buf ); 
		pthread_create( &_disk_op_thread,
		                &_op_attr,
		                page_in,
		                (void*)a_file );
		//NOTE: in order to guarantee that the file is not
		//      closed while operations are in progress,
		//      page_in() will handle this sort of things.
		_pages.pop(); //remove the used page (the file will be closed)
		                
	} else if(  !_front_buf->empty() && _back_buf->empty() ){
		//then the front buffer is full
		//and the back buffer is empty
		std::swap( _front_buf, _back_buf );
		
		//page out the back buffer, now full
		a_file = a_page_alloc( tmpfile(), _back_buf );
		pthread_create( &_disk_op_thread,
		                &_op_attr,
		                page_out,
		                (void*)a_file );
		
		//save the new page
		_pages.push( a_file->file );
	} else if( _front_buf->empty() && _back_buf->empty() ){
		//in this case, check if we have files.
		//if not, do nothing.
		if( _pages.empty() ) return;
		
		//else, load one (blocking)
		a_file = a_page_alloc( _pages.top(), _back_buf );
		page_in( a_file );
		_pages.pop();
		
		//and swap the buffers
		std::swap( _front_buf, _back_buf );
	} else if( !_front_buf->empty() && !_back_buf->empty() ){
		//then the back buffer needs flushing before swapping
		a_file = a_page_alloc( tmpfile(), _back_buf );
		page_out( a_file );
		_pages.push( a_file->file );

		std::swap( _front_buf, _back_buf );
	}
	
	a_page_free( a_file ); //a thing dies where it's born.
}

//------------------------------------------------------------------------------------
//a method to save to disk what's in the back buffer now.
//it's a voidfest to make is thread-friendly
void *r3b_ascii_paged_stack::page_out( void *a_file ){
	//first, retrieve the arguments
	a_page the_page = *(a_page*)a_file; //copy, safer
	
	//calculate the size the buffer has to be
	unsigned int buf_size;
	const unsigned int evt_info_sz = R3B_ASCII_EVENTINFO_SIZE;
	
	r3b_ascii_event *current_evt;
	void *buf;
	while( !the_page.bb->empty() ){
		//retrieve the event and calc the buffer size.
		current_evt = &the_page.bb->front();
		buf_size = r3b_ascii_event_bufsize( *current_evt );
		
		//linearize the event in the buffer
		buf = r3b_ascii_event_getbuf( *current_evt );
		
		//save it
		fwrite( buf, buf_size, 1, the_page.file );
		
		//and now, pop it and cleanup
		the_page.bb->pop_front();
		free( buf );
		
		//update the memory size
		pthread_mutex_lock( &the_page.caller->_memory_sz_mutex );
		the_page.caller->_memory_sz -= sizeof(r3b_ascii_event) +
		                               current_evt->nTracks*sizeof(r3b_ascii_track);
		pthread_mutex_unlock( &the_page.caller->_memory_sz_mutex );
	}
	
	//terminate
	return NULL;
}

//------------------------------------------------------------------------------------
//and this is a method to load a temporary file into a buffer
//again, thread-friendly
void *r3b_ascii_paged_stack::page_in( void *a_file ){
	//first, retrieve the arguments
	a_page the_page = *(a_page*)a_file; //copy, safer
	
	//calculate the size the buffer has to be
	unsigned int buf_size;
	const unsigned int evt_info_sz = R3B_ASCII_EVENTINFO_SIZE;
	
	r3b_ascii_event current_evt;
	void *buf, *track_buf;
	while( !feof( the_page.file ) ){
		//prepare the buffer for the event header, and read it
		buf = malloc( evt_info_sz );
		fread( buf, evt_info_sz, 1, the_page.file );

		//from there, we can get the information about
		//how big the whole buffer has to be
		buf_size = r3b_ascii_event_bufsize( buf );
		buf = realloc( buf, buf_size );
		track_buf = (char*)buf + evt_info_sz; //handy pointer to the tracks

		//now read the tracks
		fread( track_buf, buf_size - evt_info_sz, 1, the_page.file );

		//make the event
		current_evt = r3b_ascii_event_setbuf( buf );

		//clean up the buffer and push the event
		free( buf );
		the_page.bb->push_back( current_evt );

		//update the memory size
		pthread_mutex_lock( &the_page.caller->_memory_sz_mutex );
		the_page.caller->_memory_sz += sizeof(r3b_ascii_event) +
		                               current_evt.nTracks*sizeof(r3b_ascii_track);
		pthread_mutex_unlock( &the_page.caller->_memory_sz_mutex );
		
	}
	
	//now we are finished loading this particular file and can be closed
	//should also be automatically eliminated, since it has been created
	//by tmpfile()
	fclose( the_page.file );
	
	//terminate
	return NULL;
}
	 

