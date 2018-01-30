//a test program for the parsers

#include <stdio.h>
#include <getopt.h>

#include "../include/r3b_ascii_parsecfg.h"

int main( int argc, char **argv ){
	char ifname[256], ofname[256];
	bool ffile = false, tofile = false;
	
	char iota = 0;
	while( (iota = getopt( argc, argv, "i:o:" )) != -1 ){
		switch( iota ){
			case 'i':
				strncpy( ifname, optarg, 256 );
				ffile = true;
				break;
			case 'o':
				strncpy( ofname, optarg, 256 );
				tofile = true;
				break;
			default:
				fputs( "YOU made a mistake.\n", stderr );
				exit( 42 );
		}
	}
	
	puts( "Welcome in the parser test program!" );
	
	if( ffile ){
		FILE *ifile = fopen( ifname, "r" );
		if( !ifile ){
			fputs( "Somethig went wrong with the input file.\n", stderr );
			exit( 43 );
		}
		
		int nb_lines = r3b_ascii_parse( ifile );
		printf( "%d lines read.\n", nb_lines );
		fclose( ifile );
	}
	
	FILE *ofile = stdout;
	if( tofile ) ofile = fopen( ofname, "w" );
	
	r3b_ascii_dump_opts( ofile );
	r3b_ascii_dump_g3( ofile );
	r3b_ascii_dump_cuts( ofile );
	r3b_ascii_dump_detectors( ofile );
	
	puts( "Done. Goodbye." );
	
	return 0;
}
	
	
