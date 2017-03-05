//implementation of the buffer-handling functions in r3b_ascii_event_structs header

#include "r3b_ascii_event_structs.h"

//------------------------------------------------------------------------------------
//This function basically linearizes and then returns a long buffer with the event
//and tracks in it
void *r3b_ascii_event_getbuf( const r3b_ascii_event &event ){
	const r3b_ascii_track *t_begin = &*event.trk.begin(); //pointer to the begin of the vector
	const r3b_ascii_track *t_end = &*event.trk.end(); //past the end pointer (iterator)

	//calculate the size the buffer has to be
	unsigned int buf_size;
	const unsigned int evt_info_sz = R3B_ASCII_EVENTINFO_SIZE;
	buf_size = r3b_ascii_event_bufsize( event );
	
	//make the buffer, properly zeroed
	void *buf_proper = calloc( 1, buf_size );
	void *buf = (char*)buf_proper + 1;
	
	//copy the event info at the head of it
	memcpy( buf, &event.eventId, evt_info_sz );
	
	//copy the tracks
	memcpy( (char*)buf + evt_info_sz, t_begin, t_end-t_begin );
	
	//and we are done: mark it and return the buffer
	*(char*)buf_proper = 'E';
	return buf_proper;
}

//------------------------------------------------------------------------------------
//This much less clever function returns a buffer with a track in it
void *r3b_ascii_track_getbuf( const r3b_ascii_track &track ){
	void *buf = calloc( 1, R3B_ASCII_TRACK_BUFSIZE );
	memcpy( (char*)buf+1, &track.iPid, sizeof(r3b_ascii_track) );
	*(char*)buf = 'T';
	return buf;
}

//------------------------------------------------------------------------------------
//This function reconstructs an event from a buffer
r3b_ascii_event r3b_ascii_event_setbuf( const void *event_buf ){
	//TODO: come up with a better error handling than this!
	if( *(char*)event_buf != 'E' ){
		fprintf( stderr, "Event buffer expected, something else given." );
		exit( 10 );
	}
	
	//shift the buf one up
	event_buf = (char*)event_buf+1;
	
	//the second entry is our target, set a reference to that
	short unsigned int &n_trk = *(unsigned short int*)((unsigned int*)event_buf+1);
	const unsigned int evt_info_sz = R3B_ASCII_EVENTINFO_SIZE;
	
	//make the event
	r3b_ascii_event event = { 0, 0, 0, 0, std::vector<r3b_ascii_track>( n_trk ) };
	memcpy( &event.eventId, event_buf, evt_info_sz );
	memcpy( &*event.trk.begin(), (char*)event_buf + evt_info_sz, n_trk*sizeof(r3b_ascii_track) );
	
	return event;
}

//------------------------------------------------------------------------------------
//This considerably less clever function creates a track from a buffer
r3b_ascii_track r3b_ascii_track_setbuf( const void *track_buf ){
	if( *(char*)track_buf != 'T' ){
		fprintf( stderr, "Event buffer expected, something else given." );
		exit( 11 );
	}
	
	r3b_ascii_track track;
	memcpy( &track.iPid, (char*)track_buf+1, sizeof(r3b_ascii_track) );
	
	return track;
}

//------------------------------------------------------------------------------------
//the event bufsizer
unsigned int r3b_ascii_event_bufsize( const r3b_ascii_event &event ){
	return 	R3B_ASCII_EVENTINFO_SIZE + event.nTracks*sizeof(r3b_ascii_track) + 1;
}

//NOTE: this actually only requires the first bit of the buffer, since
//      the size of one track is constant.
unsigned int r3b_ascii_event_bufsize( const void *event_buf ){
	event_buf = (char*)event_buf + 1;
	short unsigned int &n_trk = *(unsigned short int*)((unsigned int*)event_buf+1);

	return R3B_ASCII_EVENTINFO_SIZE + n_trk*sizeof(r3b_ascii_track) + 1;
}
	
