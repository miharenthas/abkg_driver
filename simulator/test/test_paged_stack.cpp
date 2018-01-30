//this tiny program tests the paged stack, to be used in the simulator

#include <stdio.h>
#include <vector>

#include "../include/r3b_ascii_event_structs.h"
#include "../include/r3b_ascii_paged_stack.h"

int main( int argc, char **argv ){
	//make the test event
	r3b_ascii_track trk = { 1, 2, 22, 0., 1., 2., 3., 4., 5., 220 };
	r3b_ascii_event evt = { 0, 400, 10, 20, std::vector<r3b_ascii_track>( 400, trk ) };
	
	//make a paged stack
	puts( "Making a stack." );
    //set the page size
	if( argc > 1 ) r3b_pstack::page_size = atoi( argv[1] );
	else r3b_pstack::page_size = 128*1024*1024;
	r3b_pstack stk;

	//begin filling it
	printf( "Filling the stack: 0000000, 0000000" );
    int nb_events = 100000;
    if( argc > 2 ) nb_events = atoi( argv[2] );
	for( int e=0, ep=0; e < nb_events; ++e ){
		evt.eventId = e;
		stk.push( evt );
		printf( "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b%07d, %07d", e, stk.size() );
		
	}
	printf( "\nPages: %u\n", stk.nb_pages() );
	puts( "Done." );
	
	printf( "Emptying the stack: 0000000" );
	FILE *a_file = fopen( "a_file", "w" );
	for( int e=0; !stk.empty(); ++e ){
		stk.pop();
		printf( "\b\b\b\b\b\b\b%07d", e );
		fprintf( a_file, "event: %u, %hu, %f, %f\n", evt.eventId,
			                                     evt.nTracks,
			                                     evt.pBeam,
			                                     evt.b );
		for( int t=0; t < evt.nTracks; ++t )
			fprintf( a_file, "\t%d, %hu, %hhu\n\t%f, %f, %f\n\t%f, %f, %f\n\t%f\n",
			         evt.trk[t].iPid, evt.trk[t].iA, evt.trk[t].iZ,
			         evt.trk[t].px, evt.trk[t].py, evt.trk[t].pz,
			         evt.trk[t].vx, evt.trk[t].vy, evt.trk[t].vz,
			         evt.trk[t].iMass );
	}
	puts( " done." );
}
