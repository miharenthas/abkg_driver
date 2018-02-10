//this little tool merges events from up to 64 files conforming to the R3B ASCII format

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#define LINESZ 512
#define MAX_FILES 54

#define FROM_STDIN;
#define TO_STDOUT;

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
int load_horizontal( evt **evarr, FILE **streams );
void put_event( FILE *stream, evt *event );

evt *evt_alloc( int nb_lines );
void evt_resize( evt *given, int sz );
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
		strncpy( in_fname[in_fcount], argv[i] );
		++in_fcount;
	}
	
	
	char iota = 0;
	while( (iota = getopt( argc, argv, "-cr:" ) ){
		switch( iota ){
			case '-' :
				flagger |= FROM_STDIN;
				break;
			case 'c' :
				flagger |= TO_STDOUT;
				break;
		}
	}
	
	if( !in_fcount && ( flagger & TO_STDOUT ) ){
		fprintf( stderr, "%s: nothing to do here...\n" );
		exit( 0 );
	}
	
	if( in_fcount > 1 && !( flagger & TO_STDOUT ) ){
		memmove( out_fname, in_fname[in_fcount-1], 256 );
		memset( in_fname[in_fcount-1], 0, 256 );
	}
	
	FILE *in_file[MAX_FILES]; memset( in_file, 0, sizeof(FILE*) );
	for( i=0; i < in_fcount; ++i ){
		in_file[i] = fopen( in_fname[i], "r" );
		if( !in_file[i] ){
			fprintf( stderr, "%s: error: file \"%s\" not opened.\n", argv[0], in_fname[i] );
			exit( 1 );
		}
	}
	if( flagger & FROM_STDIN ) in_file[(i+1)%64] = stdin;
	
	FILE *out_file;
	if( flagger & TO_STDOUT ) out_file = stdout;
	else out_file = fopen( out_fname, "w" );
	if( !out_file ){
		fprintf( stderr, "%s: error: file \"%s\" not opened.\n", argv[0], out_fname );
		exit( 2 );
	}
	
	evt *evarr[MAX_FILES]; memset( evarr, 0, sizeof(evt*) );
	int nb_evt = 0;
	do{
		nb_evt = load_horizontal( evarr, in_file );
		for( i=0; i < nb_evt-1; ++i )
			merge( evarr[i+1], evarr[i] ); //the merged event is left in the last place
		put_event( out_file, evarr[nb_evt-1];
	} while( nb_evt );
	
	return 0;
}

//------------------------------------------------------------------------------------
//LOTS of functions!!!

//------------------------------------------------------------------------------------
//merge two events
int merge( evt *merged, const evt *one ){
	int n_tracks = 0, evt_id = 0, i;
	float b_mass, b_beta;
	sscanf( one->firstline, "%d\t%d\t%f\t%f", &evt_id, &n_tracks, &b_mass, &b_beta );
	for( i=0; i < n_tracks; ++i ) evt_push( merged, one->lines[i] );
	sprintf( merged->firstline, "%d\t%d\t%f\t%f", evt_id, merged->nl, b_mass, b_beta );
	return merged->nl;
}

//------------------------------------------------------------------------------------
//load horizontally the events (one for each file), until files are available.
int load_horizontal( evt **evarr, FILE **streams ){
	int i, t, id, nb_loaded=0;
	char cbuf[LINESZ];
	for( i=0; i < MAX_FILES; ++i ){
		if( !streams[i] ) continue; //it's critical that unused files are set to NULL!
		fgetc( streams[i] );
		if( feof( streams[i] ) ){
			fclose( streams[i] );
			streams[i] = NULL;
			continue;
		}
		fungetc( streams[i] );
		
		fgets( evarr[i]->firstline, LINESZ, streams[i] );
		sscanf( evarr[i]->firstline, "%d\t%d", &id, &evarr[i]->nl );
		evt_resize( evarr[i], evarr[i]->nl );
		for( t=0; t < evarr[i]->nl; ++t )
			fgets( evarr[i]->lines[t], LINESZ, streams[i] );
		nb_loaded++;
	}
	
	return nb_loaded;
}
			
	
	
