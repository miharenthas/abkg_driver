//This header file defines two data structures to be used within the
//modded r3b ASCII generator, in order to save space in memory

#ifndef R3B_ASCII_EVENT_STRUCTS__H
#define R3B_ASCII_EVENT_STRUCTS__H

#include <vector>
#include <stdlib.h> //for malloc & co.
#include <string.h> //memcpy & co.

//------------------------------------------------------------------------------------
//a couple of data structures to hold the event data
//NOTE: the numerical types here have been chosen to be as tight as possible
//      this is because we want to handle as many entries as possible
//      with as little memory as possible.
//      Since if not specified with precision statements (which are very
//      rarely found) the length of a float is used, that numerical type
//      should guarantee the best precision (and more speed)
//NOTE: unsigned char ranges from 0 to 255, more than enough for Z number
//      unsigned short int ranges from 0 to 65535, well beyond anything
//      circulating in the known universe as a "nucleus". If you've got tiny neutron
//      stars hanging around your accelerator, you should really give me a call.

//the track data
typedef struct _r3b_ascii_generator_custom_track{
	int iPid; //..., 4B
	unsigned short int iA; //max mass number 2^16-1, 2B
	unsigned char iZ; //max charge 255, 1B
	float px, py, pz; //single precision, 4B*3
	float vx, vy, vz; //single precision, 4B*3
	float iMass; //single precision, 4B
} r3b_ascii_track;
//Total: 35B per track

//the event structure
typedef struct _r3b_ascii_generator_custom_event{
	// Define event variable to be read from file
	unsigned int eventId; //max 2^32-1 events supported; 4B
	unsigned short int nTracks; //max 2^16-1 tracks per event supported; 2B
	float pBeam; //single precision, 4B
	float b; //single precision, 4B

	//and a vector holding the track
	std::vector<r3b_ascii_track> trk; //the track data, 35B*nTraks + 24B
} r3b_ascii_event;
//Memory needed for each event: 14+35*nTracks+24 B
//for a 32 track event: 1158B --> each GiB of memory will hold 927238 32track events.

//------------------------------------------------------------------------------------
//Two handy functions (per type) to get and set the memory buffer for IO operations
//NOTE: these function actually perform copies of the structures (and the buffers are
//      not touched, hence the const). To release the buffers obtained by the get_buffer
//      ones, you can use "free".
void *r3b_ascii_event_getbuf( const r3b_ascii_event &event );
void *r3b_ascii_track_getbuf( const r3b_ascii_track &track );

r3b_ascii_event r3b_ascii_event_setbuf( const void *event_buf )
r3b_ascii_track r3b_ascii_track_setbuf( const void *track_buf );

#endif
