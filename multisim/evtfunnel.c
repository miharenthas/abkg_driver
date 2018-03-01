//this little tool merges events from up to 64 files conforming to the R3B ASCII format

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <math.h>

#define LINESZ 512
#define LINES_PER_ALLOC 512
#define MAX_FILES 64

#define FROM_STDIN 0x01
#define TO_STDOUT 0x02
#define SLOPPY 0x04

//------------------------------------------------------------------------------------
//data structures:
typedef struct event{
	int nl;
	char firstline[LINESZ];
	char **lines;
} evt;

//------------------------------------------------------------------------------------
//functions:
int merge( evt *merged, const evt *one );
int merge_sloppy( evt *merged, const evt *one );
int load_horizontal( evt **evarr, FILE **streams );
void put_event( FILE *stream, evt *event );

evt *evt_alloc( const int nb_lines );
int gimmie_evt_size( const int nb_lines );
void evt_resize( evt *given, int sz );
void evt_clear( evt *given );
void evt_free( evt *event );
int evt_push( evt *event, const char *line );

//------------------------------------------------------------------------------------
int main( int argc, char **argv ){
	char in_fname[MAX_FILES][256];
	char out_fname[256] = "evt.dat";
	int flagger = 0;
	int in_fcount = 0;
	
	//everything that doesn't begin with -
	//is a filename in this world
	int i;
	for( i=1; i < argc && i < MAX_FILES-1; ++i ){
		if( argv[i][0] == '-' ) continue;
		strncpy( in_fname[in_fcount], argv[i], LINESZ );
		++in_fcount;
	}
	
	
	char iota = 0;
	while( (iota = getopt( argc, argv, "-cs" ) ) != -1 ){
		switch( iota ){
			case '-' :
				flagger |= FROM_STDIN;
				break;
			case 'c' :
				flagger |= TO_STDOUT;
				break;
			case 's' :
				flagger |= SLOPPY;
				break;
		}
	}
	
	if( !in_fcount && ( flagger & TO_STDOUT ) ){
		fprintf( stderr, "%s: nothing to do here...\n", argv[0] );
		exit( 0 );
	}
	
	if( in_fcount > 1 && !( flagger & TO_STDOUT ) ){
		memmove( out_fname, in_fname[in_fcount-1], 256 );
		memset( in_fname[in_fcount-1], 0, 256 );
		--in_fcount;
	}
	
	FILE *in_file[MAX_FILES]; memset( in_file, 0, sizeof(FILE*) );
	for( i=0; i < in_fcount; ++i ){
		in_file[i] = fopen( in_fname[i], "r" );
		if( !in_file[i] ){
			fprintf( stderr, "%s: error: file \"%s\" not opened.\n", argv[0], in_fname[i] );
			exit( 1 );
		}
	}
	if( flagger & FROM_STDIN ) in_file[(i+1)%MAX_FILES] = stdin;
	
	FILE *out_file;
	if( flagger & TO_STDOUT ) out_file = stdout;
	else out_file = fopen( out_fname, "w" );
	if( !out_file ){
		fprintf( stderr, "%s: error: file \"%s\" not opened.\n", argv[0], out_fname );
		exit( 2 );
	}
	
	evt *evarr[MAX_FILES]; memset( evarr, 0, sizeof(evt*) );
	int nb_evt = 0, ck = 0;
	do{
		nb_evt = load_horizontal( evarr, in_file );
		for( i=0; i < nb_evt-1; ++i ){
			if( flagger & SLOPPY ) merge_sloppy( evarr[i+1], evarr[i] ); //the merged event is left in the last place
			else{
				ck = merge( evarr[i+1], evarr[i] );
				if( ck < 0 ){
					fputs( "evtfunnel: incompatible events.\n", stderr );
					exit( 3 );
				}
			}
		}
		if( nb_evt ) put_event( out_file, evarr[nb_evt-1] );
	} while( nb_evt );
	
	return 0;
}

//------------------------------------------------------------------------------------
//LOTS of functions!!!

//------------------------------------------------------------------------------------
//put the event on a stream
void put_event( FILE *stream, evt *event ){
	int i;
	fputs( event->firstline, stream );
	for( i=0; i < event->nl; ++i ) fputs( event->lines[i], stream );
}

//------------------------------------------------------------------------------------
//merge two events
int merge( evt *merged, const evt *one ){
	int n_tracks = 0, evt_id = 0, i;
	float b_mass, b_mass_m, b_beta, b_beta_m;
	sscanf( one->firstline, "%d\t%d\t%f\t%f", &evt_id, &n_tracks, &b_mass, &b_beta );
	sscanf( merged->firstline, "%d\t%d\t%f\t%f", &evt_id, &n_tracks, &b_mass_m, &b_beta_m );
	if( b_mass != b_mass_m || b_beta != b_beta_m ) return -1;
	for( i=0; i < n_tracks; ++i ) evt_push( merged, one->lines[i] );
	sprintf( merged->firstline, "%d\t%d\t%f\t%f\n", evt_id, merged->nl, b_mass, b_beta );
	return merged->nl;
}

//merge two events without checking if it makes sense (take tke first relevant line from the one)
int merge_sloppy( evt *merged, const evt *one ){
	int n_tracks = 0, evt_id = 0, i;
	float b_mass, b_beta;
	sscanf( one->firstline, "%d\t%d\t%f\t%f", &evt_id, &n_tracks, &b_mass, &b_beta );
	for( i=0; i < n_tracks; ++i ) evt_push( merged, one->lines[i] );
	sprintf( merged->firstline, "%d\t%d\t%f\t%f\n", evt_id, merged->nl, b_mass, b_beta );
	return merged->nl;
}

//------------------------------------------------------------------------------------
//load horizontally the events (one for each file), until files are available.
int load_horizontal( evt **evarr, FILE **streams ){
	int i, t, id, nb_loaded=0;
	char cbuf[LINESZ], c;
	for( i=0; i < MAX_FILES; ++i ){
		if( !streams[i] ) continue; //it's critical that unused files are set to NULL!
		c = fgetc( streams[i] );
		if( feof( streams[i] ) ){
			fclose( streams[i] );
			streams[i] = NULL;
			continue;
		}
		ungetc( c, streams[i] );
		
		if( evarr[nb_loaded] ) evt_clear( evarr[nb_loaded] );
		else evarr[nb_loaded] = evt_alloc( 1 );
		
		fgets( evarr[nb_loaded]->firstline, LINESZ, streams[i] );
		sscanf( evarr[nb_loaded]->firstline, "%d\t%d", &id, &evarr[i]->nl );
		evt_resize( evarr[nb_loaded], evarr[nb_loaded]->nl );
		for( t=0; t < evarr[nb_loaded]->nl; ++t )
			fgets( evarr[nb_loaded]->lines[t], LINESZ, streams[i] );
		nb_loaded++;
	}
	
	return nb_loaded;
}

//------------------------------------------------------------------------------------
//memory fiddlers

int gimmie_evt_size( const int nb_lines ){
	return ceil( (float)nb_lines/LINES_PER_ALLOC )*LINES_PER_ALLOC;
}

evt *evt_alloc( const int nb_lines ){
	int lta = gimmie_evt_size( nb_lines );
	evt *the_evt = calloc( 1, sizeof( evt ) );
	the_evt->nl = nb_lines;
	the_evt->lines = (char**)calloc( lta, sizeof( char* ) );
	int i;
	for( i=0; i < lta; ++i ) the_evt->lines[i] = (char*)calloc( LINESZ, 1 );
	return the_evt;
}
	
void evt_resize( evt *given, const int sz ){
	int lta = gimmie_evt_size( sz );
	int al = gimmie_evt_size( given->nl );
	if( lta != al ){
		given->lines = (char**)realloc( given->lines, lta*sizeof( char* ) );
		int i;
		for( i=al; i < lta; ++i ) given->lines[i] = (char*)calloc( LINESZ, 1 );
	}
	given->nl = sz;
}

void evt_clear( evt *event ){
	int ltc = gimmie_evt_size( event->nl );
	int i;
	memset( event->firstline, 0, LINESZ );
	for( i=0; i < ltc; ++i ) memset( event->lines[i], 0, LINESZ );
}

void evt_free( evt *event ){
	int al = gimmie_evt_size( event->nl );
	int i;
	for( i=0; i < al; ++i ) free( event->lines[i] );
	free( event->lines );
	free( event );
}

int evt_push( evt *event, const char *line ){
	int al = gimmie_evt_size( event->nl );
	if( al == event->nl ) evt_resize( event, event->nl+1 );
	strncpy( event->lines[event->nl], line, LINESZ );
	event->nl++;
	return event->nl;
}
