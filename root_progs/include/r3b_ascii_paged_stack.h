//this class is a stack (FIFO) that pages out to disk when a certain amount of data
//is reached. So far, it is tailored to work with r3b_ascii_event elements, but it might
//be made more flexible in the future. Very optionally.

#ifndef R3B_ASCII_PAGED_stack__H
#define R3B_ASCII_PAGED_stack__H

#include <deque>
#include <stack>
#include <algorithm>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

#include "r3b_ascii_event_structs.h"

#define DEFAULT_PAGE_SIZE 1610612736 //the default page size (1.5 GiB)
                                     //NOTE: this indicates the number of
                                     //      elements that will reside

//a macro to check the pthread error code.
#define RCK( rc ) \
if( rc ){ \
	fprintf( stderr, "r3b_pstack: error: this happened %d at __LINE__.\n", rc ); \
	exit( rc ); \
}

typedef class r3b_ascii_paged_stack {
	public:
		//constructors:
		//default only (see note below).
		r3b_ascii_paged_stack();
		
		//destructor
		~r3b_ascii_paged_stack();
		
		//stack operations: push and pop
		//push: returns the size of the stack in memory
		//      receives a reference of an event structur
		unsigned int push( r3b_ascii_event &given );
		//pop: returns a brand new event structure
		//     taking it from the beginning of the stack. 
		r3b_ascii_event pop();
		//top: just look on top of the stack
		r3b_ascii_event top();
		//check for emptiness:
		//front buf and pages must be empty
		inline bool empty(){ return _front_buf->empty() && _pages.empty(); };
		
		//check the paging status
		inline unsigned int own_page_size(){ return _own_page_sz; };
		inline unsigned int size_in_memory(){ return _memory_sz; };
		inline unsigned int nb_pages(){ return _pages.size(); };
		
		//a static that defines the current page size.
		//I'm doing it like this because it's unlikely to change
		//during the program and passing it to the constructor
		//feels cumbersome.
		//Note that a copy of this variable is made every time
		//a stack is created, and for that stack the size remains
		//constant.
		static long unsigned int page_size; //it is initialized as DEFAULT_PAGE_SIZE
	protected:
		//a handy data structure
		//declared protected: no need for the rest
		//of the program to be aware of it.
		typedef struct _r3b_ascii_paged_stack_page {
			r3b_ascii_paged_stack *caller; //the reference to the caller object
			std::deque<r3b_ascii_event> *bb; //copy the address of the back buffer
			FILE *file; //the stream (will be a temporary file)
		} a_page;

		//and some allocation/deallocation routines.
		//note that tmp_file_free frees memory but does NOT close the file!
		a_page *a_page_alloc( FILE *file, std::deque<r3b_ascii_event> *bb );
		void a_page_free( a_page *a_file );
		
		std::stack<FILE*> _pages; //keep track of the created files
		                          //NOTE: files that have made it
		                          //      are read only!
		
		std::deque<r3b_ascii_event> *_front_buf; //the buffer exposed to the program
		std::deque<r3b_ascii_event> *_back_buf; //the buffer exposed to disk
		
		pthread_t _disk_op_thread; //the thread that will handle the disk operations.
		pthread_attr_t _op_attr; //attributes for the disk operator.
		pthread_mutex_t _mem_sz_mutex; //a lock on the memory size
		pthread_mutexattr_t _mem_sz_mutexattr; //the attribute thereof
		
		const unsigned int _own_page_sz; //own page size, a constant
		long unsigned int _memory_sz; //current size in memory
		bool _op_busy; //busy flag of the disk operator
		
		char swap_buffers(); //swap front and back buffer
		//these are static because they have to end up into threads
		static void *page_out( void *a_file ); //dumps the back buffer to disk
		static void *page_in( void *a_file ); //retrieve the first file on disk
	private:
		//NOTE: these two copy instruments are here because we are using temporary
		//      files and I didn't have time to work out a sensible way to solve
		//      the ownership.
		r3b_ascii_paged_stack( r3b_ascii_paged_stack &given ): _own_page_sz( 0 ){};
		r3b_ascii_paged_stack &operator=( r3b_ascii_paged_stack &right ){};
} r3b_pstack;

#endif
