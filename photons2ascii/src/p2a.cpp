//this program takes in some photons as per the nuclear realization thingie puts them
//and makes ascii-generator friendly events

#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <pthread.h>

#include <algorithm>

#include "p2a_angtools.h"
#include "p2a_gen_angdist.h"
#include "p2a_dboost.h"
#include "p2a_gen_momenta.h"
#include "p2a_make_evt.h"

#define VERBOSE 0x01
#define FROM_FILE 0x02
#define TO_FILE 0X03
#define GESPREAD 0x10

#define PHOTON_BUNCH_SZ 4096

//------------------------------------------------------------------------------------
//a single entry in the photon buffer: it's a salami of CoM energies
//and their number -- the salami itself is allocated at will. 
struct photons{
	unsigned nb;
	float* p;
};
void photons_alloc( struct photons *pht, const unsigned nb ){
	pht->p = (float*)malloc( sizeof(float)*nb );
	pht->nb = nb;
}
void photons_realloc( struct photons *pht, const unsigned nb ){
	pht->p = (float*)realloc( pht->p, sizeof(float)*nb );
	pht->nb = nb;
}
void photons_free( struct photons *pht ){ free( pht->p ); };

//and a structure to pass in a thread, with pointers to the buffers
//since this will be passed as a reference, I can later tamper with
//the read and write buffers to move them around where I need them
struct tmessage{
	p2a::event *evt_buf; //write buffer -- in general, this will not be touched
	unsigned evt_buf_sz; //its size
	struct photons *pht_buf; //read buffer -- this will be swapped
	unsigned pht_buf_sz; //its size
	bool worker_go_on;
};

//------------------------------------------------------------------------------------
//some worker functions:
//fills an already allocated buffer with nb events
unsigned fill_photon_buffer( struct photons *pbuf, FILE *stream, unsigned nb );
//the processor of a stream
unsigned process( FILE *out, FILE *in );
//worker thread routine
void *make_events( void *the_tmessage );

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
	float beam_sigma = 0;
	gsl_vector *beam_dir = gsl_vector_alloc( 3 );
	gsl_vector_set( beam_dir, 0, 0 );
	gsl_vector_set( beam_dir, 1, 0 );
	gsl_vector_set( beam_dir, 3, 1 ); //Z is the default direction.
	p2a::fromto ft = { 0, 2*__pi, 0, __pi };

	struct option opts[] = {
		{ "verbose", no_argument, &flagger, flagger | VERBOSE },
		{ "beam-a", required_argument, NULL, 'A' },
		{ "beam-z", required_argument, NULL, 'Z' },
		{ "beam-energy", required_argument, NULL, 'e' },
		{ "beam-versor", required_argument, NULL, 'd' },
		{ "solid-angle", required_argument, NULL, 'a' },
		{ "gaussian-energy-spread", required_argument, NULL, 'g' },
		{ "input", required_argument, NULL, 'i' },
		{ "output", required_argument, NULL, 'o' },
		{ NULL, 0, NULL, 0 }
	};
	
	char iota = 0; int idx;
	while( (iota = getopt_long( argc, argv, "vA:Z:e:d:a:g:i:o:", opts, &idx )) != -1 ){
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
			case 'a' :
				sscanf( optarg, "from[%f,%f]to[%f,%f]",
					ft.th_from, ft.th_to,
					ft.ph_from, ft.ph_to );
				float *ft_ptr = &ft.th_from;
				for( int i=0; i < 4; ++i ) ft_ptr[i] *= __pi/180;
				break;
			case 'g' :
				flagger |= GESPREAD;
				beam_sigma = atof( optarg );
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
	
	//let's do walking processing, just because
	FILE *instream = NULL;
	struct tmessage msg;
	if( flagger & TO_FILE ) outstream = fopen( out_fname, "w" );
	else outstream = stdout;
	if( flagger & FROM_FILE ){
		for( int f=0; f < in_fcount; ++f ){
			insteam = fopen( in_fname[f], "r" );
			if( instream && outstream ) msg = process( outstream, instream );
			else exit( 42 );
		}
	} else msg = process( outstream, stdin );
			
	return 0;
}

//------------------------------------------------------------------------------------
//the processor job:
//read some events from the stream
//pass it to the worker thread for processing
//swap buffers
//repeat until no events are read-able
unsigned process( FILE *out, FILE *in ){
	//buffers: front and back and event
	struct photon pbuf_A[PHOTON_BUNCH_SZ], *pbuf_front = pbuf_A;
	struct photon pbuf_B[PHOTON_BUNCH_SZ], *pbuf_back = pbuf_B;
	
	struct tmessage msg = { NULL, 0, pbuf_front, PHOTON_BUNCH_SZ, true };
	
	//lock-read
	//start worker thread
	
	unsigned nb_read=0, nb_total=0;
	nb_read = fill_photon_buffer( pbuf_front, in, PHOTON_BUNCH_SZ );
	//unlock-read
	while( nb_read ){
		nb_total += nb_read;
		
		//buffer swap
		//lock-read
		std::swap( pbuf_front, pbuf_back );
		msg.pht_buf = pbuf_front;
		msg.pht_buf_sz = nb_read;
		//unlock-read
		
		//lock-write
		for( int i=0; i < msg.evt_buf_sz; ++i )
			p2a::put_event( out, msg.evt_buf[i] );
		//unlock-write
		
		nb_read = fill_photon_buffer( pbuf_back, in, PHOTON_BUNCH_SZ );
	}
	msg.worker_go_on = false;
	
	return nb_total;
}

//------------------------------------------------------------------------------------
//worker thread
void *make_events( void *the_msg ){
	struct tmessage *msg = (struct tmessage*)the_msg;
	
	while( msg->worker_go_on ){
		

//------------------------------------------------------------------------------------
//fill the photon buffer
unsigned fill_photon_buffer( struct photons *pbuf, FILE *stream, unsigned nb ){
	unsigned read=0;
	char line[1024], *phot;
	
	int pcount=0, pnb=0;
	for( int i=0; i < nb; ++i ){
		fgets( line, 1024, stream );
		if( feof( stream ) ) break;
		
		pnb=8;
		photon_alloc( &pbuf[i], pnb );
		
		phot = strtok( line, "," );
		pcount = 0;
		while( phot ){
			pbuf[i].p[pcount] = atof( phot );
			phot = strtok( NULL, "," );
			++pcount;
			if( pcount >= pnb ){
				pnb *= 2;
				photon_realloc( &pbuf[i], pnb );
			}
		}
		photon_realloc( &pbuf[i], pcount+1 );
		++read;
	}
	
	return read;
}
