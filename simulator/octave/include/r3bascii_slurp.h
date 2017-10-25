//definition of the loader for the ascii events

#ifndef O_R3BASCII_SLURP__H
#define O_R3BASCII_SLURP__H

#include <vector>

#include <stdio.h>
#include <stdlib.h>

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
//NOTE: these have been largely copied from the structures in r3b_ascii_gen.h
//      the difference is the way events reference tracks.

//the track data
typedef struct _r3b_ascii_generator_custom_track{
	int iPid; //..., 4B*nTracks(?)
	unsigned short int iA; //max mass number 2^16-1, 2B
	unsigned char iZ; //max charge 255, 1B
	float px, py, pz; //single precision, 4B*3
	float vx, vy, vz; //single precision, 4B*3
	float iMass; //single precision, 4B
} r3b_ascii_track;
//Total: 35B per track

//the event structure
typedef struct _r3b_ascii_generator_custom_event_alternative{
	// Define event variable to be read from file
	unsigned int eventId; //max 2^32-1 events supported; 4B
	unsigned short int nTracks; //max 2^16-1 tracks per event supported; 2B
	float pBeam; //single precision, 4B
	float b; //single precision, 4B

	//and a vector holding the track
	short unsigned int first_track; //pointer(-ish) to the first track of the event, 2B
} r3b_ascii_event_alt;

//------------------------------------------------------------------------------------
//function definitions
//the meat of the file:
//    FILE **infiles: array of pointer to streams (files or pipes)
//    unsigned int n_files: the dimension thereof.
//returns:
//    void **et: two pointers to two vectors:
//               et[0] ---> std::vector<r3b_ascii_event_alt>*
//               et[1] ---> std::vector<r3b_ascii_track>*
void **r3bascii_slurp( FILE **infiles, unsigned int n_files );

//a helper function that deallocates et.
void r3bascii_slurp_free( void **et );

#endif


