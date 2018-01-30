//this program provides an interface for the actual simulator engine.
//Based on the customized ASCII event generator.
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <map>

#include "r3bsim_driver.h"

int main( int argc, char **argv ){
	//globber_optsme name holders (one file only, one shot deal)
	char in_fname[128];
	char conf_fname[128] = "config.cfg";
	char cfgsave_fname[128] = "config.cfg";
	char out_fname[128] = "r3bsim.root";
	char par_fname[128] = "r3bpar.root";
	std::map<std::string, std::string> m;
	bool use_config = false;
	bool save_config = false;
	bool use_absolute_paths = false;
	bool infile = false;
	bool verbose = false;
	
	//retrieve the input files before the options
	for( int i=1; i < argc; ++i ){
		if( argv[i][0] != '-' ){
			if( strlen( argv[i] ) < 128 ){
				strcpy( in_fname, argv[i] );
				infile = true;
			} else {
				fprintf( stderr, "File name too long.\n" );
				exit( 1 );
			}
		} else break;
	}
	
	r3bsim_options_init();
	
	//long options. Where it can apply, the corresponding characted
	//is the same as in "r3bsim_fmt"
	struct option l_opts[] = {
		{ "detector-list", required_argument, NULL, 'B' },
		{ "event-file", required_argument, NULL, 'i' },
		{ "nb-events", required_argument, NULL, 'n' },
		{ "parameter-file", required_argument, NULL, 'p' },
		{ "output-file", required_argument, NULL, 'o' },
		{ "magnet-current", required_argument, NULL, 'H' },
		{ "no-magnet", no_argument, NULL, 'G' },
		{ "save-geometry", no_argument, NULL, 'D' },
		{ "target-material", required_argument, NULL, 'C' },
		{ "verbose", no_argument, NULL, 'v' },
		{ "help", no_argument, NULL, 'h' },
		{ "cut-ugly", required_argument, NULL, 'M' },
		{ "mem-limit", required_argument, NULL, 'L' },
		{ "field-scale", required_argument, NULL, 'N' },
		{ "config-file", required_argument, NULL, 'F' },
		{ "use-absolute-paths", no_argument, NULL, 'a' },
		{ "save-config", required_argument, NULL, 'c' },
		{ NULL, 0, NULL, 0 }
	};
	
	//parse with getopt
	char iota=0;
	int opt_idx;
	const char short_opts[] = "i:n:p:o:vB:H:D:G:C:M:hF:ac:";
	while( ( iota = getopt_long( argc, argv, short_opts, l_opts, &opt_idx ) ) != -1 ){
		switch( iota ){
			case 'i' : //sets one input file (overwrite the unoptioned one)
				if( strlen( optarg ) < 128 ){
					infile = true;
					strcpy( in_fname, optarg );
				} else {
					fprintf( stderr, "Too much input.\n" );
					exit( 1 );
				}
				break;
			case 'o' : //sets the output name
				if( strlen( optarg ) < 128 ){
					strcpy( out_fname, optarg );
				} else {
					fprintf( stderr, "File name too long.\n" );
					exit( 1 );
				}
				break;
			case 'p' : //same behavior as above, with the parfiles
				if( strlen( optarg ) < 128 ){
					strcpy( par_fname, optarg );
				} else {
					fprintf( stderr, "File name too long.\n" );
					exit( 1 );
				}
				break;
			case 'n' : //sets the number of events to be processed
				r3bsim_options_edit( (r3bsim_fmt*)L"A@", atoi( optarg ) );
				break;
			case 'B' : //parses and sets the detector list
				m = r3bsim_detmant( optarg );
				r3bsim_options_edit( (r3bsim_fmt*)L"B@", &m );
				break;
			case 'H' : //sets the current for this measure 
				r3bsim_options_edit( (r3bsim_fmt*)L"H@", atof( optarg ) );
				break;
			case 'D' : //doesn't save the geometry information
			           //for event visualization
				r3bsim_options_edit( (r3bsim_fmt*)L"D@", true );
				break;
			case 'C' : //set the target material
				r3bsim_options_edit( (r3bsim_fmt*)L"C@", optarg );
				break;
			case 'G' : //we have a magnet, so set the flag
				r3bsim_options_edit( (r3bsim_fmt*)L"G@", false );
				break;
			case 'v' : //set the verobse output
				verbose = true;
				r3bsim_options_edit( (r3bsim_fmt*)L"L@", true );
				break;
			case 'M' : //set the ugly cut macro
				r3bsim_options_edit( (r3bsim_fmt*)L"M@", optarg );
				break;
			case 'L' : //limit the memory
				sscanf( optarg, "%lu", &r3b_pstack::page_size );
				break;
			case 'N' : //set the field scale
				r3bsim_options_edit( (r3bsim_fmt*)L"N@", atof( optarg ) );
				break;
			case 'h' : //displays the help
				system( "cat doc/r3bsim_sim_ascii_help" );
				exit( 0 );
				break;
			case 'F':
				strncpy( conf_fname, optarg, 128 );
				use_config = true;
				break;
			case 'a':
				use_absolute_paths = true;
				break;
			case 'c' :
				strncpy( cfgsave_fname, optarg, 128 );
				save_config = true;
				break;
			default : //default: exit 
				fprintf( stderr, "Unknown option(s). Type --help for info.\n" );
				exit( 1 );
				break;
		}
	}

	if( verbose ){
		puts( "***Welcome in the R3B ASCII simulation driver!***" );
		puts( "Setting up the environment... " );
	}
	
	//do some environment awareness stuff
	if( !use_absolute_paths ) r3bsim_env_setup();
	else{
		setenv( "GEOMPATH", "/", 1 );
		setenv( "CONFIG_DIR", "/", 1 );
	}
	
	//open the input stream
	if( verbose ) puts( "Opening the input stream..." );
	FILE *in_stream;
	if( infile ){ //then we are reading from a file
		in_stream = fopen( in_fname, "r" );
		if( in_stream == NULL ){
			fprintf( stderr, "File %s not reachable.\n", in_fname );
			exit( 1 );
		}
	} else { //it's a pipe from stdin
		in_stream = stdin;
	}
	
	//set the IO options (the config file will override these if they are given)
	r3bsim_options_edit( (r3bsim_fmt*)L"KIJ@", in_stream, out_fname, par_fname );
	
	//use the config file if one is given
	FILE *configfile;
	if( use_config ){
		configfile = fopen( conf_fname, "r" );
		if( configfile ) r3b_ascii_parse( configfile );
		else{
			fprintf( stderr, "File \"%s\" couldn't be opened.\n", conf_fname );
			exit( 1 );
		}
		fclose( configfile );
		globber_opts.EventFile = in_stream;
	}
	
	FILE *savecfile;
	if( save_config ){
		savecfile = fopen( cfgsave_fname, "w" );
		if( !savecfile ){
			fprintf( stderr, "couldn't open \"%s\".\n", cfgsave_fname );
			exit( 2 );
		}
		r3b_ascii_dump_opts( savecfile );
		r3b_ascii_dump_cuts( savecfile );
		r3b_ascii_dump_g3( savecfile );
		r3b_ascii_dump_detectors( savecfile );
		fclose( savecfile );
	}
	
	//we are completely set, let's go
	if( verbose ) r3b_ascii_dump_opts( stdout );
	float wall_clock = r3bsim_driver( globber_opts );
	
	if( verbose ){
		puts( "Simulation ended. If you didn't get error messages, all is OK." );
		printf( "Wall clock runtime: %f.\n", wall_clock );
	}
	
	//happy thoughts
	return 0;
}
	
	
	
