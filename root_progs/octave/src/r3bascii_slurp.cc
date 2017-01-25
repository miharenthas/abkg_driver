//this function loads the tracks and the events in two vectors of data structures.
//ready for conversion by r3bascii_conver_{events,tracks}.

#include "r3bascii_slurp.h"

//------------------------------------------------------------------------------------
//the big deal of this file: the loader function.
//the return type is an array (of two) of pointers to void
//which actually are the pointers.
//NOTE: this is a rework of r3b_ascii_gen::slurp.
void **r3bascii_slurp( FILE **infiles, unsigned int so_many ){
	//declare and initialize an event holder
	r3b_ascii_event_alt evt;
	r3b_ascii_track trk;
	int current_first_track;

	void **et = (void**)malloc( 2*sizeof(void*) );
	et[0] = new std::vector<r3b_ascii_event_alt>;
	et[1] = new std::vector<r3b_ascii_track>;
	
	//lopp on the files
	for( unsigned int f=0; f < so_many; ++f ){
		//loop on the events
		current_first_track = 1; //octave indexing is dumb.
		for( unsigned int i=0; !feof( infiles[f] ) ; ++i ){
			//get the event header
			fscanf( infiles[f], "%u\t%hu\t%f\t%f", &evt.eventId,
				      &evt.nTracks, &evt.pBeam, &evt.b );
		
			evt.first_track = current_first_track;
			current_first_track += evt.nTracks;
			//loop over the tracks of the current event
			for( int t=0; t < evt.nTracks; ++t ){
				fscanf( infiles[f], "%d\t%hhu\t%hu\t%f\t%f\t%f\t%f\t%f\t%f\t%f",
					&trk.iPid, &trk.iZ, &trk.iA, &trk.px, &trk.py,
					&trk.pz, &trk.vx, &trk.vy, &trk.vz, &trk.iMass );
			
				//push the track
				((std::vector<r3b_ascii_track>*)et[1])->push_back( trk );
			} //end of track loop
		
			((std::vector<r3b_ascii_event_alt>*)et[0])->push_back( evt );
		} //end of the event loop
	} //end of the file loop
	
	return et;
}

//------------------------------------------------------------------------------------
//and this function simply frees the two buffers in a safe manner
void r3bascii_slurp_free( void **et ){
	delete (std::vector<r3b_ascii_event_alt>*)et[0];
	delete (std::vector<r3b_ascii_track>*)et[1];
}
