//this tool goes from a momentum (and a bunch of other information) to an event
//compatible with the custom r3b ascii generator included in this package
//NOTE: might break the "official" one, untested.

#ifndef P2A_MAKE_EVT__H
#define P2A_MAKE_EVT__H

#include <string.h>
#include <stdio.h>

#include <vector>

#include "p2a_dboost.h"

#ifndef __proton_mass
#define __proton_mass 0.9382720814 //GeV/c^2
#endif
#ifndef __neutron_mass
#define __neutron_mass 0.9395654135 //GeV/c^2
#endif

namespace p2a{
	//----------------------------------------------------------------------------
	//NOTE: these two structures are a copy from
	//      $ABKG_HOME/root_progs/include/r3b_ascii_event_structs.h
	//      which I don't include because I didn't make that particular
	//      toolkit into a library. Will probably change.
	//the track data
	typedef struct _r3b_ascii_generator_custom_track{
		int iPid; //..., 4B
		unsigned short int iA; //max mass number 2^16-1, 2B
		unsigned char iZ; //max charge 255, 1B
		float px, py, pz; //single precision, 4B*3
		float vx, vy, vz; //single precision, 4B*3
		float iMass; //single precision, 4B
	} track;
	//Total: 35B per track

	//the event structure
	typedef struct _r3b_ascii_generator_custom_event{
		// Define event variable to be read from file
		unsigned int eventId; //max 2^32-1 events supported; 4B
		unsigned short int nTracks; //max 2^16-1 tracks per event supported; 2B
		float pBeam; //single precision, 4B
		float b; //single precision, 4B

		//and a vector holding the track
		std::vector<track> trk; //the track data, 35B*nTraks + 24B
	} event;
	//Memory needed for each event: 14+35*nTracks+24 B
	//for a 32 track event: 1158B --> each GiB of memory will hold 927238 32track events.
	
	//----------------------------------------------------------------------------
	//a class to keep track of the event ID
	class the_event_ID{
		private:
			static unsigned _id;
		public:
			static void init( unsigned id=0 ){ _id = id; };
			friend void make_event_hdr( event &evt, float beam_A,
			                            float beam_Z, float beam_nrg );
	};
	
	//----------------------------------------------------------------------------
	//functions to make a track out of a momentum,
	//copy tracks into an event (in case <event>.trk = tracks is too hard)
	//make its header
	//and putting it to a stream
	track photon2track( const gsl_vector *mom );
	void cpy_tracks( event &evt, const std::vector<track> &tracks );
	//NOTE: beam mass is supposed to be in UMA, while the energy in AGeV
	//      in the event will be written the values for the beam momentum
	//      and the corresponding beta.
	void make_event_hdr( event &evt, float beam_A, float beam_Z, float beam_nrg );
	void put_event( FILE *stream, event &evt );
}

#endif
