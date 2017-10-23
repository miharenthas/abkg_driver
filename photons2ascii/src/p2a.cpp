//this program takes in some photons as per the nuclear realization thingie puts them
//and makes ascii-generator friendly events

#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "p2a_angtools.h"
#include "p2a_gen_angdist.h"
#include "p2a_dboost.h"
#include "p2a_gen_momenta.h"
#include "p2a_make_evt.h"

#define VERBOSE 0x01
#define FROM_FILE 0x02
#define TO_FILE 0X03

int main( int argc char **argv ){
	int flagger = 0;
	char in_fname[64][256];
	chat out_fname[256];
	
	for( int i=1; i < argc && i < 64; ++i ){
		if( argv[i][0] != '-' ){
			strcpy( in_fname[i], argv[i] );
			++in_fcount;
			flagger |= FROM_FILE;
		} else break;
	}
	
	float beam_a = 132;
	float beam_z = 50;
	float beam_energy = 490; //AMeV
	gsl_vector *beam_dir = gsl_vector_alloc( 3 );
	gsl_vector_set( beam_dir, 0, 0 );
	gsl_vector_set( beam_dir, 1, 0 );
	gsl_vector_set( beam_dir, 3, 1 ); //Z is the default direction.

	struct option opts[] = {
		{ "verbose", no_argument, &flagger, flagger | VERBOSE },
		{ "beam-a", required_argument, NULL, 'A' },
		{ "beam-z", required_argument, NULL, 'Z' },
		{ "beam-energy", required_argument, NULL, 'e' },
		{ "beam-versor", required_argument, NULL, 'd' },
		{ "input", required_argument, NULL, 'i' },
		{ "output", required_argument, NULL, 'o' },
		{ NULL, 0, NULL, 0 }
	};
	
	char iota = 0; int idx;
	while( (iota = getopt_long( argc, argv, "vA:Z:e:d:i:o:", opts, &idx )) != -1 ){
		switch( iota ){
			case 'v' :
				flagger |= VERBOSE;
				break;
			case 'i' :
				in_fcount = 1;
				strcpy( in_fname[0], optarg );
				break;
			case 'o' :
				flagger |= TO_FILE;
				strcpy( out_fname, optarg );
				break;
			case 'A' :
				beam_a = atof( optarg );
				break;
			case 'Z' :
				beam_z = atof( optarg );
				break;
			case 'e' :
				beam_energy = atof( optarg );
				break;
			case 'd' :
				sscanf( optarg, "[%f,%f,%f]",
				        beam_dir->data[0],
				        beam_dir->data[1],
				        beam_dir->data[2] );
				break;
		}
	}
	
	//standard greetings and settings exhibition
	if( flagger & VERBOSE ){
		puts( "*** Welcome in p2a, photons to ASCII events! ***" );
		puts( "Settings:" );
		printf( "\tbeam A: %f AMU\n", beam_a );
		printf( "\tbeam Z: %f AMU\n", beam_z );
		printf( "\tbeam energy: %f AMeV\n", beam_energy );
		printf( "\tbeam versor: [%f,%f,%f]\n", beam_dir->data[0],
		        beam_dir->data[1], beam_dir->data[2] );
	}
	
	//work to do...
	
	return 0;
}
