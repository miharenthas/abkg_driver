//this little program multiplexes or demultiplexes events
//from one file to many or from many to one.
//always interleaved, one event per file.
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#define CBUF_SIZE 512 //the size of a buffer for strings.
#define FNAME_SIZE 256 //the maximum size of the file name
#define MAX_NB_FILES 64 //how many input/output files we are supporting

//------------------------------------------------------------------------------------
//functions:
void one_to_many( int argc, char **argv );
void many_to_one( int argc, char **argv );

//an utility to check the feofs of many files
int check_feof( FILE **files, int n_files );

//------------------------------------------------------------------------------------
int main( int argc, char **argv ){
	int demux_flag = 1; //1: one to many files, 0: many to one file.

	//check for options (only short)
	char iota = 0;
	while( (iota = getopt( argc, argv, "mh" )) != -1 ){
		switch( iota ){
			case 'm' :
				demux_flag = 0;
				break;
			case 'h' :
				printf( "Usage: evtmux [-m] <file names> [--]\n" );
				exit( 0 );
				break;
		}
	}
	
	//now, decide what to do
	if( demux_flag ) one_to_many( argc, argv );
	else many_to_one( argc, argv );
	
	
	return 0;
}

//------------------------------------------------------------------------------------
//one ASCII event file to many ASCII event files
void one_to_many( int argc, char **argv ){
	char in_fname[FNAME_SIZE] = "nonenonenone";
	char out_fnames[MAX_NB_FILES][FNAME_SIZE];

	//look if we have the stdin/out flag set
	int from_stdin = 0;
	for( int i=1; i < argc; i++ ){
		if( strstr( argv[i], "--" ) ){
			from_stdin = 1;
			break;
		}
	}

	//get the names of the files:
	//first, the input (it's the firs file)
	for( int f=1; f < argc; f++ ){
		if( strlen( argv[f] ) > FNAME_SIZE ){
			fprintf( stderr, "evtmus: error: file name too long\n" );
			exit( 1 );
		}
		if( argv[f][0] != '-' && !from_stdin ){
			strcpy( in_fname, argv[f] );
			break;
		}
	}

	//second, the outputs
	int fcount = 0;
	for( int f=1; f < argc && fcount < MAX_NB_FILES; f++ ){
		if( strcmp( argv[f], in_fname ) != 0 && argv[f][0] != '-' ){
			strcpy( out_fnames[fcount], argv[f] );
			fcount++;
		}
	}

	//the files...
	FILE *in_file, *out_files[MAX_NB_FILES];
	
	//check the names and open the files/pipes
	if( strstr( in_fname, "nonenonenone" ) || from_stdin ) in_file = stdin;
	else in_file = fopen( in_fname, "r" );

	if( !fcount ){ out_files[0] = stdout; fcount = 1; }
	else for( int f=0; f < fcount; f++ ) out_files[f] = fopen( out_fnames[f], "w" );

	//now, do the demuxing
	int n_tracks = 0, evt_id, evt = 0;
	char cbuf[CBUF_SIZE];
	while( !feof( in_file ) ){
		//get the first line, the event line
		//get the number of events and then write it to the file
		fgets( cbuf, CBUF_SIZE, in_file );
		sscanf( cbuf, "%d\t%d", &evt_id, &n_tracks );
		if( !feof( in_file ) ) fputs( cbuf, out_files[evt%fcount] );
		else break;
		
		//write the tracks
		for( int l=0; l < n_tracks; ++l ){
			fgets( cbuf, CBUF_SIZE, in_file );
			fputs( cbuf, out_files[evt%fcount] );
		}
		
		evt++;
	}

	//close all the files
	if( in_file != stdin ) fclose( in_file );
	if( out_files[0] != stdout ) for( int f=0; f < fcount; f++ ) fclose( out_files[f] );
}

//------------------------------------------------------------------------------------
//many ASCII event files to one
void many_to_one( int argc, char **argv ){
	char out_fname[FNAME_SIZE] = "nonenonenone";
	char in_fnames[MAX_NB_FILES][FNAME_SIZE];
	
	//look if we have the stdin/out flag set
	int to_stdout = 0;
	for( int i=1; i < argc; i++ ){
		if( strstr( argv[i], "--" ) ){
			to_stdout = 1;
			break;
		}
	}
	
	//get the names of the files:
	//first, the output (from the back, it's the last)
	for( int f=argc-1; f >= 1; f-- ){
		if( strlen( argv[f] ) > FNAME_SIZE ){
			fprintf( stderr, "evtmus: error: file name too long\n" );
			exit( 1 );
		}
		if( argv[f][0] != '-' ){
			strcpy( out_fname, argv[f] );
			break;
		}
	}
	//second, the outputs
	int fcount = 0;
	for( int f=1; f < argc && fcount < MAX_NB_FILES; f++ ){
		if( strcmp( argv[f], out_fname ) != 0 && argv[f][0] != '-' ){
			strcpy( in_fnames[fcount], argv[f] );
			fcount++;
		}
	}

	//the files...
	FILE *out_file, *in_files[MAX_NB_FILES];

	//check the names and open the files/pipes
	if( strstr( out_fname, "nonenonenone" ) || to_stdout ) out_file = stdout;
	else out_file = fopen( out_fname, "w" );

	if( !fcount ){ in_files[0] = stdout; fcount = 1; }
	else for( int f=0; f < fcount; f++ ) in_files[f] = fopen( in_fnames[f], "r" );

	//now, do the demuxing
	int n_tracks = 0, ibuf, evt = 0;
	char cbuf[CBUF_SIZE];
	while( !check_feof( in_files, fcount ) ){
		//get the first line, the event line
		//get the number of events and then write it to the file

		if( !feof( in_files[evt%fcount] ) ){
			fgets( cbuf, CBUF_SIZE, in_files[evt%fcount] );

			sscanf( cbuf, "%d\t%d", &ibuf, &n_tracks );
			if( !feof( in_files[evt%fcount] ) ) fputs( cbuf, out_file );
		} else { evt++; continue; }
		
		//write the tracks
		for( int l=0; l < n_tracks; ++l ){
			fgets( cbuf, CBUF_SIZE, in_files[evt%fcount] );
			fputs( cbuf, out_file );
		}
		
		evt++;
	}

	//close all the files
	if( out_file != stdin ) fclose( out_file );
	if( in_files[0] != stdout ) for( int f=0; f < fcount; f++ ) fclose( in_files[f] );
}

//------------------------------------------------------------------------------------
//check the foef of many files
int check_feof( FILE **files, int n_files ){
	int feofs = 0;
	for( int f=0; f < n_files; f++ ) if( feof( files[f] ) ) feofs++;
	return feofs == n_files;
}

