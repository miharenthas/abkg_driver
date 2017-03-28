//a tiny program to test the toolkit of r3b_ascii_event_structs

#include <stdio.h>
#include <vector>
#include "../include/r3b_ascii_event_structs.h"

int main( int argc, char **argv ){
	//make an event
	r3b_ascii_track trk = { 1, 2, 22, 0., 1., 2., 3., 4., 5., 220 };
	r3b_ascii_event evt = { 42, 5, 10, 20, std::vector<r3b_ascii_track>( 5, trk ) };

	//print it
	printf( "event: %u, %hu, %f, %f\n", evt.eventId,
	                                    evt.nTracks,
	                                    evt.pBeam,
	                                    evt.b );
	for( int t=0; t < evt.nTracks; ++t )
		printf( "\t%d, %hu, %hhu\n\t%f, %f, %f\n\t%f, %f, %f\n\t%f\n",
		        evt.trk[t].iPid, evt.trk[t].iA, evt.trk[t].iZ,
		        evt.trk[t].px, evt.trk[t].py, evt.trk[t].pz,
		        evt.trk[t].vx, evt.trk[t].vy, evt.trk[t].vz,
		        evt.trk[t].iMass );
	
	puts( "Getting event buffer." );
	void *buf = r3b_ascii_event_getbuf( evt );
	
	puts( "Getting track buffer." );
	void *tbuf = r3b_ascii_track_getbuf( trk );
	
	printf( "Buffer sizes: %u, %u\n",
	        r3b_ascii_event_bufsize( evt ),
	        r3b_ascii_event_bufsize( buf ) );
	
	puts( "Rebuilding the event." );
	evt = r3b_ascii_event_setbuf( buf );
	
	puts( "Rebuilding the track." );
	trk = r3b_ascii_track_setbuf( tbuf );
	
	//print it
	printf( "event: %u, %hu, %f, %f\n", evt.eventId,
	                                    evt.nTracks,
	                                    evt.pBeam,
	                                    evt.b );
	for( int t=0; t < evt.nTracks; ++t )
		printf( "\t%d, %hu, %hhu\n\t%f, %f, %f\n\t%f, %f, %f\n\t%f\n",
		        evt.trk[t].iPid, evt.trk[t].iA, evt.trk[t].iZ,
		        evt.trk[t].px, evt.trk[t].py, evt.trk[t].pz,
		        evt.trk[t].vx, evt.trk[t].vy, evt.trk[t].vz,
		        evt.trk[t].iMass );
	
	return 0;
}
