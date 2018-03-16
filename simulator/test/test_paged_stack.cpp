//this tiny program tests the paged stack, to be used in the simulator

#include <stdio.h>
#include <vector>

#include <unistd.h>
#include <stdio.h>
#include "../include/r3b_ascii_event_structs.h"
#include "../include/r3b_ascii_paged_stack.h"

r3b_ascii_event slurp_event( FILE *stream ){
	//get the event header
	r3b_ascii_event evt = {
		0,
		32,
		0.,
		0.,
		std::vector<r3b_ascii_track>( 32 )
	};
	
	fscanf( stream, "%u\t%hu\t%f\t%f", &evt.eventId,
	        &evt.nTracks, &evt.pBeam, &evt.b );
		
	if( feof( stream ) ){
		r3b_ascii_event dummy = {
			0,
			0,
			0.,
			0.,
			std::vector<r3b_ascii_track>( 0 )
		};
		return dummy;
	}
		
	//resize the vectors if necessary
	evt.trk.resize( evt.nTracks );
		
	//loop over the tracks of the current event
	for( int t=0; t < evt.nTracks; ++t ){
		fscanf( stream, "%d\t%hhu\t%hu\t%f\t%f\t%f\t%f\t%f\t%f\t%f",
		&evt.trk[t].iPid, &evt.trk[t].iZ, &evt.trk[t].iA, &evt.trk[t].px, &evt.trk[t].py,
		&evt.trk[t].pz, &evt.trk[t].vx, &evt.trk[t].vy, &evt.trk[t].vz, &evt.trk[t].iMass );
		
	} //end of track loop
	
	return evt;
}

int main( int argc, char **argv ){
	//make the test event
	r3b_ascii_track trk = { 1, 0, 22, 0., 1., 2., 3., 4., 5., 220 };
	r3b_ascii_event evt = { 0, 400, 10, 20, std::vector<r3b_ascii_track>( 400, trk ) };
    int nb_events = 100000;
	int read_from_src = 0;
	FILE *src = stdin, *a_file = stdout;
	r3b_pstack::page_size = 128*1024*1024;
	
	char iota = 0;
	while( (iota = getopt( argc, argv, "p:n:o:cf:" )) != -1 ){
		switch( iota ){
			case 'p' :
				r3b_pstack::page_size = atoi( optarg );
				break;
			case 'n' :
				nb_events = atoi( optarg );
				break;
			case 'o' :
				a_file = fopen( optarg, "w" );
				if( !a_file ){
					fprintf( stderr, "can't open %s\n", optarg );
					exit( 2 );
				}
				break;
			case 'c' :
				read_from_src = 1;
				break;
			case  'f' :
				read_from_src = 1;
				src = fopen( optarg, "r" );
				if( !src ){
					fprintf( stderr, "can't open %s\n", optarg );
					exit( 1 );
				}
				break;
		}
	}
	
	//make a paged stack
	if( a_file != stdout ) puts( "Making a stack." );
    //set the page size
	r3b_pstack stk;

	//begin filling it
	if( a_file != stdout ) printf( "Filling the stack: 0000000, 0000000" );
	for( int e=0; e < nb_events; ++e ){
		if( read_from_src ) evt = slurp_event( src );
		else evt.eventId = e;
		stk.push( evt );
		if( a_file != stdout ) printf( "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b%07d, %07d", e, stk.size() );
		
	}
	if( a_file != stdout ) printf( "\nPages: %u\n", stk.nb_pages() );
	if( a_file != stdout ) puts( "Done." );
	
	if( a_file != stdout ) printf( "Emptying the stack: 0000000" );
	for( int e=0; !stk.empty(); ++e ){
		stk.pop();
		if( a_file != stdout ) printf( "\b\b\b\b\b\b\b%07d", e );
		fprintf( a_file, "%u\t%hu\t%f\t%f\n", evt.eventId,
			                                     evt.nTracks,
			                                     evt.pBeam,
			                                     evt.b );
		for( int t=0; t < evt.nTracks; ++t )
			fprintf( a_file, "%d\t%hu\t%hhu\t%f\t%f\t%f\t%f\t%f\t%f\t\t%f\n",
			         evt.trk[t].iPid, evt.trk[t].iZ, evt.trk[t].iA,
			         evt.trk[t].px, evt.trk[t].py, evt.trk[t].pz,
			         evt.trk[t].vx, evt.trk[t].vy, evt.trk[t].vz,
			         evt.trk[t].iMass );
	}
	fclose( src );
	fclose( a_file );
	if( a_file != stdout ) puts( " done." );
}
