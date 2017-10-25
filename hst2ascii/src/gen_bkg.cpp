//this program drives "gen_bkg_from_histo.C" and allowes to
//read and write to the standard input and output

#include "gen_bkg_from_histo.h"

#include <stdio.h>
#include <getopt.h>
#include <string.h>

//ROOT includes
#include "TFile.h"
#include "TH2.h"

#define OUT_TO_MANY 129

int main( int argc, char **argv ){
	//first thing first, parse the command line
	//consider every argument before the options
	//as an input file name. Count them also.
	char in_fname[128][128];
	char out_fname[128][128];
	char in_hname[32] = "h1";
	int in_fcount = 0, out_flag = 0;
	
	//run through the argument vector
	for( int i=1; i < argc && argv[i][0] != '-'; ++i ){
		if( strlen( argv[i] ) < 128  )
			strcpy( in_fname[i-1], argv[i] );
		else{
			fprintf( stderr, "%s: File name too long.\n", argv[0] );
			exit( 1 );
		}
		++in_fcount;
	}
	
	//delcare some gear to parse the options
	char iota=0; int option_idx=0;
	int verbose=0, debug=0, bzip2=1; //flags
	gen_bkg_opts *go = gen_bkg_options_alloc();
	
	//the option list
	struct option options[] = {
		{ "verobse", no_argument, NULL, 'v' },
		{ "debug", no_argument, NULL, 'd' },
		{ "input-file", required_argument, NULL, 'i' },
		{ "output-ascii", required_argument, NULL, 'o' },
		{ "output-many", no_argument, NULL, 'O' },
		{ "target-histogram", required_argument, NULL, 'T' },
		{ "nb-events", required_argument, NULL, 'D' },
		{ "tot-cs", required_argument, NULL, 'E' },
		{ "target-thickness", required_argument, NULL, 'F' },
		{ "target-density", required_argument, NULL, 'G' },
		{ "target-molar-mass", required_argument, NULL, 'H' },
		{ "help", no_argument, NULL, 'h' },
		{ "no-compression", no_argument, NULL, 'c' },
		{ 0, 0, 0, 999 }
	};
	
	//parsing loop
	while( (iota = getopt_long( argc, argv, "dvhi:o:c", options, &option_idx )) != -1 ){
		switch( iota ){
			case 'v' : //set verbose flag
				verbose = true;
				break;
			case 'd' : //set debug flag
				debug = true;
				break;
			case 'i' : //this should be treated as a legacy option
			           //also, forces the processing of one single
			           //histogram file.
			  if( strlen( optarg ) < 128 )
					strcpy( in_fname[0], optarg );
				else {
					fprintf( stderr, "%s: Input file name too long.\n", argv[0] );
					exit( 1 );
				}
				in_fcount = 1;
				break;
			case 'o' : //sets the output file
			           //NOTE: not specifyng anything causes the program
			           //      to print on the standard output
				if( strlen( optarg ) < 128 )
					strcpy( out_fname[0], optarg );
				else {
					fprintf( stderr, "%s: Output file name too long.\n", argv[0] );
					exit( 1 );
				}
				out_flag = true;
				break;
			case 'O' : //output as many files as the inputs
				   //adding adding a prefix to them
				out_flag = OUT_TO_MANY;
				break;
			case 'T' : //set the name of the histogram that will be read
				if( strlen( optarg ) < 32 )
					strcpy( in_hname, optarg );
				else{
					fprintf( stderr, "%s: Histogram name too long.\n", argv[0] );
					exit( 1 );
				}
				break;
			case 'D' : //sets the maximum number of events
				go->evnts = atoi( optarg );
				break;
			case 'E' : //sets the total crossection
				go->tot_cs = atof( optarg );
				break;
			case 'F' : //sets the target thickness
				go->target_thickness = atof( optarg );
				break;
			case 'G' : //sets the target density
				go->target_density = atof( optarg );
				break;
			case 'H' : //sets the target molar mass
				go->target_molmass = atof( optarg );
				break;
			case 'c' : //no compression
				bzip2 = 0;
				break;
			case 'h' : //display an help and exits
				system( "cat \"doc/gen_bkg_help\"" );
				exit( 0 );
			default : //error message and exit.
				fprintf( stderr,
				         "Uknown option \"%c\". Type --help for more info.\n",
				         optopt );
				exit( 1 );
		}
	}
	
	//standard greetings
	if( verbose ) printf( "*** Weclome to the root-to-ascii converter for ABKG!***\n" );

	//check that input files have been specified
	//NOTE: no piping of root files, sorry.
	if( !in_fcount ){
		fprintf( stderr, "%s: No input files given.\n", argv[0] );
		exit( 1 );
	}
	
	//check if we are outputting to many files
	//if so, change the suffix from .root to .dat
	//and use the same rest of the name.
	char *suffix;
	if( out_flag == OUT_TO_MANY ){
		if( verbose ) printf( "Gathering output file names information:\n" );
		for( int i=0; i < in_fcount; ++i ){
			strcpy( out_fname[i], in_fname[i] );
			suffix = strstr( out_fname[i], ".root" );
			if( suffix == NULL ){
				fprintf( stderr, "%s: %s is an invalid input file.\n",
				         argv[0], out_fname[i] );
				exit( 1 );
			}
			strcpy( suffix, ( bzip2 )? ".bz2" : ".dat" );
			if( verbose ) printf( "\tFile \"%s\" will be written.\n", out_fname[i] );
			
		}
	} else if( verbose ) printf( "Output file: \"%s\".\n", out_fname[0] );
	
	//if we made it to here, we can loop on the input files
	//NOTE: it is assumed that all the results will be saved into the same
	//      output file, unless otherwise specified.
	if( verbose ){
		printf( "Entering main loop.\n" );
	}
	FILE *out_file;
	TFile *in_file;
	TH2 *in_histogram;
	char pcommand[170];
	for( int f=0; f < in_fcount; ++f ){
		if( verbose ){
			printf( "\tProcessing file \"%s\"...\n", in_fname[f] );
		}
		in_file = TFile::Open( in_fname[f] );
		
		//NOTE: in the ROOT documentation there's no mention of the
		//      behaviour of this function in case "h1" doesn't exist.
		in_histogram = NULL; //because I'm checking on it later.
		in_histogram = (TH2*)in_file->Get( in_hname );
		if( in_histogram == NULL || !in_histogram->GetEffectiveEntries() ){
			fprintf( stderr, "%s: No or empty histogram \"%s\" in file %s, Skipping...\n",
			         argv[0], in_hname, in_fname[f] );
			continue;
		}
		
		//open the output file
		if( bzip2 && out_flag ){
			if( out_flag == OUT_TO_MANY ){
				strcpy( pcommand, "bzip2 -z > " );
				strcat( pcommand, out_fname[f] );
				if( verbose ){
					printf( "\tOutput to file: \"%s\".\n", out_fname[f] );
				}
			} else {
				strcpy( pcommand, "bzip2 -z >> " );
				strcat( pcommand, out_fname[0] );
				if( verbose ){
					printf( "\tOutput to file: \"%s\".\n", out_fname[0] );
				}
			}
			out_file = popen( pcommand, "w" );
		} else if( out_flag ) {
			out_file = ( out_flag == OUT_TO_MANY )?
				fopen( out_fname[f], "w" ) :
				fopen( out_fname[0], "w+" );
		} else out_file = stdout;
		
		//set the the output target and the histogram in the options
		gen_bkg_fmt format[] = {
			HISTO,
			( bzip2 )? A_PIPE : A_FILE,
			VERBOSE,
			END_OF_FORMAT
		};
		gen_bkg_options_edit( go, format, in_histogram, out_file, verbose );
		
		//launch the conversion
		if( verbose ) printf( "\tLaunching conversion...\n===\n" );
		gen_bkg_from_histo( *go );
		if( verbose ) printf( "===\n\tDone.\n" );
		
		//close pipes/files
		fflush( out_file );
		( bzip2 )? pclose( out_file ) : fclose( out_file );
		in_file->Close();
	}
	
	//free the options
	gen_bkg_options_free( go );
	
	//happy thoughts
	if( verbose ) printf( "***Done. Goodbye.***\n" );
	return 0;
}
		
		
