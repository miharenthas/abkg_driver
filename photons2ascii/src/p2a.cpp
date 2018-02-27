//this program takes in some photons as per the nuclear realization thingie puts them
//and makes ascii-generator friendly events

#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#if !(defined(__APPLE__) && defined(__clang__))
	#include <omp.h>
#endif

#include <algorithm>

#include "p2a_angtools.h"
#include "p2a_gen_angdist.h"
#include "p2a_dboost.h"
#include "p2a_gen_momenta.h"
#include "p2a_make_evt.h"

#define VERBOSE 0x01
#define FROM_FILE 0x02
#define TO_FILE 0X04
#define GESPREAD 0x10

#define PHOTON_BUNCH_SZ 4096
#define MAX_MOMENTUM_NORM 1000 //GeV, or one TeV
#define MAX_ATTEMPTS 512 //number of attempts to rescue a failed momentum before giving up

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
//and all sort of goodies, like all the relevant optionZ.
//since this will be passed as a reference, I can later tamper with
//the read and write buffers to move them around where I need them
struct tmessage{
	p2a::event *evt_buf; //write buffer -- in general, this will not be touched
	unsigned evt_buf_sz; //its size
	struct photons *pht_buf; //read buffer -- this will be swapped
	unsigned pht_buf_sz; //its size
	bool worker_go_on;
	float beam_a;
	float beam_z;
	float beam_e; //AMeV
	float beam_sigma;
	gsl_vector *beam_dir;
	p2a::fromto ft;
};

//------------------------------------------------------------------------------------
//some worker functions:
//fills an already allocated buffer with nb events
unsigned fill_photon_buffer( struct photons *pbuf, FILE *stream, unsigned nb );
//the processor of a stream
unsigned process( FILE *out, FILE *in, struct tmessage *msg );
//worker thread routine
void *make_events( void *the_tmessage );

int main( int argc, char **argv ){
	int flagger = 0, in_fcount=0;
	char in_fname[64][256];
	char out_fname[256];
	
	for( int i=1; i < argc && i < 64; ++i ){
		if( argv[i][0] != '-' ){
			strcpy( in_fname[in_fcount], argv[i] );
			++in_fcount;
			flagger |= FROM_FILE;
		} else break;
	}
	
	struct tmessage msg;
	
	msg.beam_a = 132;
	msg.beam_z = 50;
	msg.beam_e = 0.490; //AMeV
	msg.beam_sigma = 0;
	msg.beam_dir = gsl_vector_alloc( 3 );
	gsl_vector_set( msg.beam_dir, 0, 0 );
	gsl_vector_set( msg.beam_dir, 1, 0 );
	gsl_vector_set( msg.beam_dir, 2, 1 ); //Z is the default direction.
	msg.ft = { 0, 2*__pi, 0, __pi }; //phi and theta

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
	
	char iota = 0; int idx; float *ft_ptr;
	while( (iota = getopt_long( argc, argv, "vA:Z:e:d:a:g:i:o:", opts, &idx )) != -1 ){
		switch( iota ){
			case 'v' :
				flagger |= VERBOSE;
				break;
			case 'i' :
				in_fcount = 1;
				strcpy( in_fname[0], optarg );
				flagger |= FROM_FILE;
				break;
			case 'o' :
				flagger |= TO_FILE;
				strcpy( out_fname, optarg );
				break;
			case 'A' :
				msg.beam_a = atof( optarg );
				break;
			case 'Z' :
				msg.beam_z = atof( optarg );
				break;
			case 'e' :
				msg.beam_e = atof( optarg );
				break;
			case 'd' :
				sscanf( optarg, "[%f,%f,%f]",
				        msg.beam_dir->data[0],
				        msg.beam_dir->data[1],
				        msg.beam_dir->data[2] );
				break;
			case 'a' :
				sscanf( optarg, "from[%f,%f]to[%f,%f]",
					msg.ft.th_from, msg.ft.ph_from,
					msg.ft.th_to, msg.ft.ph_to );
				ft_ptr = &msg.ft.th_from;
				for( int i=0; i < 4; ++i ) ft_ptr[i] *= __pi/180;
				break;
			case 'g' :
				flagger |= GESPREAD;
				msg.beam_sigma = atof( optarg );
				fprintf( stderr, "p2a: warning: -g option has no effect, yet.\n" );
				break;
		}
	}
	
	//fast calculate the total beam energy
	msg.beam_e *= msg.beam_a;
	
	//standard greetings and settings exhibition
	if( flagger & VERBOSE ){
		puts( "*** Welcome in p2a, photons to ASCII events! ***" );
		puts( "Settings:" );
		printf( "\tbeam A: %f AMU\n", msg.beam_a );
		printf( "\tbeam Z: %f AMU\n", msg.beam_z );
		printf( "\tbeam energy: %f AGeV\n", msg.beam_e/msg.beam_a );
		printf( "\tbeam versor: [%f,%f,%f]\n", msg.beam_dir->data[0],
		        msg.beam_dir->data[1], msg.beam_dir->data[2] );
	}
	
	//let's do walking processing, just because
	FILE *instream = NULL, *outstream=NULL;
	if( flagger & VERBOSE ) puts( "Processing..." );
	if( flagger & TO_FILE ) outstream = fopen( out_fname, "w" );
	else outstream = stdout;
	if( !outstream ) exit( 41 );
	unsigned nb_proc = 0;
	srand( time(NULL) ); //init the random number generator.
	if( flagger & FROM_FILE ){
		for( int f=0; f < in_fcount; ++f ){
			instream = fopen( in_fname[f], "r" );
			if( !instream ) exit( 42 );
			if( flagger & VERBOSE )
				printf( "Reading file %s.\n", in_fname[f] );
			nb_proc += process( outstream, instream, &msg );
		}
	} else nb_proc = process( outstream, stdin, &msg );
	if( flagger & VERBOSE ) printf( "Events read: %d.\n", nb_proc );
	
	if( flagger & VERBOSE ) puts( "*** Done, goodbye. ***" );	
	return 0;
}

//------------------------------------------------------------------------------------
//the processor job:
//read some events from the stream
//pass it to the worker thread for processing
//swap buffers
//repeat until no events are read-able
unsigned process( FILE *out, FILE *in, struct tmessage *msg ){
	//buffers: front and back and event
	struct photons *pbuf_front =
		(struct photons*)calloc( sizeof(struct photons), PHOTON_BUNCH_SZ );
	struct photons *pbuf_back =
		(struct photons*)calloc( sizeof(struct photons), PHOTON_BUNCH_SZ );
	
	msg->evt_buf = NULL;
	msg->evt_buf_sz = 0;
	msg->pht_buf = pbuf_front;
	msg->pht_buf_sz = PHOTON_BUNCH_SZ;
	msg->worker_go_on = true;
	
	//lock-read
	//start worker thread
	
	unsigned nb_read=0, nb_total=0;
	nb_read = fill_photon_buffer( pbuf_back, in, PHOTON_BUNCH_SZ );

	//unlock-read
	while( nb_read ){
		nb_total += nb_read;
		
		//buffer swap
		//lock-read
		std::swap( pbuf_front, pbuf_back );
		msg->pht_buf = pbuf_front;
		msg->pht_buf_sz = nb_read;
		//unlock-read
		
		make_events( msg );
		
		//lock-write
		for( int i=0; i < msg->evt_buf_sz; ++i )
			p2a::put_event( out, msg->evt_buf[i] );
		//unlock-write
		
		nb_read = fill_photon_buffer( pbuf_back, in, PHOTON_BUNCH_SZ );
	}
	msg->worker_go_on = false;
	
	free( pbuf_front );
	free( pbuf_back );
	
	return nb_total;
}

//------------------------------------------------------------------------------------
//worker
//NOTE: this has been though as a thread thing, but I can't be bothered yet
//      some notes on how the thing should be handled are already in the code
//      leaving them inside, waiting for the will to implement them.
void *make_events( void *the_msg ){
	struct tmessage *msg = (struct tmessage*)the_msg;
	gsl_vector *dir, *mom, *mom4;
	p2a::angpair pair;
	gsl_rng *rng;
	
	gsl_vector *ranges = gsl_vector_alloc( 4 );
	p2a::get_ranges( ranges, msg->ft );
	
	//create a shared null vector
	gsl_vector *nullv = gsl_vector_alloc( 3 );
	memset( nullv->data, 0, 3*sizeof(double) );
	
	float e, beta;
	int failcount;
	
	//NOTE: the while loop is effectively something needed by the pthreaded app
	//while( msg->worker_go_on ){
	//lock-read //lock-write
	msg->evt_buf = (p2a::event*)realloc( msg->evt_buf,
		sizeof(p2a::event)*(msg->pht_buf_sz) );
	msg->evt_buf_sz = msg->pht_buf_sz;
	
	//init a unforum pseudorandom distro
	srand( time(NULL) );
	
	#pragma omp parallel private( beta, pair, dir, mom, mom4, e, rng, failcount )
	{
	rng = gsl_rng_alloc( gsl_rng_default );
	#if !(defined(__APPLE__) && defined(__clang__))
		gsl_rng_set( rng, rand() + omp_get_thread_num() );
	#else
		gsl_rng_set( rng, rand() );
	#endif
	dir = gsl_vector_alloc( 3 );
	mom = gsl_vector_alloc( 3 );
	mom4 = gsl_vector_alloc( 4 );
	#pragma omp for
	for( int i=0; i < msg->pht_buf_sz; ++i ){
		//prepare the event
		msg->evt_buf[i].trk = std::vector<p2a::track>( msg->pht_buf[i].nb );
		msg->evt_buf[i].nTracks = msg->pht_buf[i].nb;
		p2a::make_event_hdr( msg->evt_buf[i],
		                     msg->beam_a,
		                     msg->beam_z,
		                     msg->beam_e );
		
		//do the photonZ
		for( int j=0; j < msg->pht_buf[i].nb; ++j ){
			//if a momentum comes out not well
			//try again from here
			failcount = 0;
			__TRY_AGAIN_SAM__ :
			
			p2a::get_randpair( pair, ranges, msg->ft );
			p2a::get_randdir( dir, pair );
			if( msg->beam_sigma )
				e = p2a::get_rande( msg->beam_e, msg->beam_sigma, rng );
			else e = msg->beam_e;
			beta = p2a::beam2beta( e, msg->beam_a, msg->beam_z );
			p2a::get_momentum( mom, dir, msg->pht_buf[i].p[j] );
			p2a::boost( mom4, mom, msg->beam_dir, beta );
			p2a::fourmom2mom( mom, mom4 );
			if( gsl_blas_dnrm2( mom ) < MAX_MOMENTUM_NORM )
				msg->evt_buf[i].trk[j] = p2a::photon2track( mom );
			else{
				++failcount;
				if( failcount < MAX_ATTEMPTS ) goto __TRY_AGAIN_SAM__;
				msg->evt_buf[i].trk[j] = p2a::photon2track( nullv );
			}
		}
	}
	gsl_vector_free( dir );
	gsl_vector_free( mom );
	gsl_rng_free( rng );
	} //end of parallel pragma
	//unlock-write //unlock-read
	//}
	
	//pthread_exit( NULL )
	return NULL;
}

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
		photons_realloc( &pbuf[i], pnb );
		
		phot = strtok( line, "," );
		pcount = 0;
		while( phot ){
			pbuf[i].p[pcount] = atof( phot )/1000; //MeV to GeV
			phot = strtok( NULL, "," );
			++pcount;
			if( pcount >= pnb-1 ){
				pnb *= 2;
				photons_realloc( &pbuf[i], pnb );
			}
		}
		photons_realloc( &pbuf[i], pcount+1 );
		++read;
	}
	
	return read;
}
