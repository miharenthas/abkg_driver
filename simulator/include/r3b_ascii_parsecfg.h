//this tool will parse a config file and produce the configuration object
//that will then fly around the program --either passed or globbed

#ifndef R3B_ASCII_PARSECFG__H
#define R3B_ASCII_PARSECFG__H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>

#include <map>
#include <string>
#include <vector>

//------------------------------------------------------------------------------------
//Some handy data structure (just to keep the options in order
//and not having to send around humongous amount of parameters)
typedef struct r3bsim_general_setup_options {
	int nEvents;
	std::map<std::string, std::string> fDetlist;
	char Target[128];
	bool fVis;
	char fMC[128];
	bool fUserPList;
	bool fR3BMagnet;
	double fMeasCurrent;
	char OutFile[128];
	char ParFile[128];
	FILE *EventFile;
	bool verbose;
	char usr_cuts[128];
	float field_scale;
	char material_file[128];
} r3bsim_opts;

//the format descriptor enum
typedef enum r3bsim_options_format {
	END_OF_FORMAT = '@', //terminates the format string, wide character @
	NB_EVENTS, //defines the number of events, wide character A
	DET_LIST_PTR, //passes (a pointer to) the detector list, wide character B
	TARGET, //defines the target material, wide character C
	VISUALIZATION_READY, //makes the output visualizable, wide character D
	MC_ENGINE, //sets the monte carlo engine, wide characted E
	USER_PLIST, //..., wide character F
	HAS_MAGNET, //do we have a magnet in this simulation?, wide character G
	MEASURE_CURRENT, //magnet's current for the measure, wide character H
	OUT_FILE, //sets the name of the output ROOT file, wide character I
	PAR_FILE, //sets the name of the parameter file, wide character J
	EVENT_FILE, //sets the name of the event file, wide character K
	VERBOSE, //sets the verbose flag, wide character L
	USR_CUTS, //sets the user cuts maro path, wide character M
	FIELD_SCALE, //sets the field scale, wide character N
	MATERIAL_FILE //sets the material file, wc O
} r3bsim_fmt;

//------------------------------------------------------------------------------------
//some more options, this time for the various MC engines

struct r3bsim_MC_settings {
//nothing yet...
};

//geant 3 specific -- you should add stuff if necessary, never take it out.
//remember to update the parser as well!
//NOTE: the defaults are mutuated from R3BRoot/gconfig/g3Config.C
#define G3_MAX_OPT_LENGTH 3
typedef struct r3bsim_geant3_settings : public r3bsim_MC_settings {
	//Settages for the engine itself
	//for reference see geant321 doc and TGeant3
	int trig = 1;
	int swit[2] = {4, 10};
	int debu[3] = {0, 0, 1};
	int gauto = 1;
	int aban = 0;
	int opti = 2;
	float eran = 5.e-7;
	
	//physics (!)
	char ckov = 1;
	char rayl = 1;
	char pair = 1;
	char comp = 1;
	char phot = 1;
	char pfis = 0;
	char dray = 1;
	char anni = 1;
	char brem = 1;
	char hadr = 5;
	char munu = 1;
	char dcay = 1;
	char loss = 1;
	char muls = 1;
	char stra = 1;
	char sync = 0;
} r3bsim_g3setup;

//settings for geant4 and fluka will follow, at some pointer
typedef struct r3bsim_geant4_settings : public r3bsim_MC_settings {
//nothing yet..
} r3bsim_g4setup;

typedef struct r3bsim_fluka_settings : public r3bsim_MC_settings {
//nothing yet...
} r3bsim_Fsetup;

//------------------------------------------------------------------------------------
//cutZ
struct r3bsim_cuts {
	float gamma = 1e-4;
	float electron = 1e-3;
	float neutral_had = 1e-4;
	float charged_had = 1e-3;
	float muon = 1e-3 ;
	float electron_bremss = 1e-3;
	float muon_bremss = 1e-3;
	float electron_delta = 1e-3;
	float muon_delta = 1e-3;
	float muon_pair = 1e-3;
	float max_ToF = 1e10;
};

//====================================================================================
//utils

//------------------------------------------------------------------------------------
//old utils from r3bsim_driver: handle the option structure and parse the command line detector list
/*************************************************************************************
                                   NOTE ON THE FORMAT
in this implementation, a string containing the detector information.
the DETECTOR NAMES are should be:
    -given in ALL CAPS
    -enclosed in columns: ":ALADIN:"
    -when more than one version is specified, it **must** be part of the
     detector name and follow it: ":CALIFAv14a:". If multiple versions are
     available but none is specified, the latest is selected.
Some detectors (the target) have options. These should be unique and can occur
anywhere in the string --although it's good practice to have them near the detector spec.

NOTE: this implementation was the lowest hanging fruit.
      Probably smarter things are coming.
*************************************************************************************/
std::map<std::string, std::string> r3bsim_detmant( const char *det_opts ); //parse a detector description.
std::string r3bsim_make_geofname( const std::string &spec ); //make a file name
void r3bsim_env_setup(); //sets up the environment
void r3bsim_options_init(); //allocate the options with the default values
void r3bsim_options_edit( const r3bsim_fmt *format, ... ); //edit the options based on a format string

//------------------------------------------------------------------------------------
//A whole bunch of parsers

//section parser (also the top dog)
//this will begin the file (with a default section on)
//and the call the appropriate parser for that section
//the file is passed down and the line-pointer preserved.
int r3b_ascii_parse( FILE *stream );

//cut parser
void r3b_ascii_parse__cuts( FILE *stream, int &records );

//geant3 settings parser
void r3b_ascii_parse__g3( FILE *stream, int &records );

//geant 4 and fluka, TODO
void r3b_ascii_parse__g4( FILE *stream, int &records );
void r3b_ascii_parse__fluka( FILE *stream, int &records );

//finally, a reimplementation of the detector list
//so we can use config files
void r3b_ascii_parse__detectors( FILE *stream, int &records );
void r3b_ascii_parse__dlist( FILE *stream, int &records );

//last but not (?) least, the option parser
void r3b_ascii_parse__opts( FILE *stream, int &records );

//------------------------------------------------------------------------------------
//some dumpers. These functions (pretty) print the data structures
//to some stream.
void r3b_ascii_dump_opts( FILE *stream );
void r3b_ascii_dump_cuts( FILE *stream );
void r3b_ascii_dump_g3( FILE *stream );
void r3b_ascii_dump_detectors( FILE *stream );

//------------------------------------------------------------------------------------
//this is gonna be controversial, but sadly not really avoidable:
//settings are made global. Don't mess them with threading.
extern r3bsim_g3setup globber_g3;
extern r3bsim_cuts globber_cuts;
extern r3bsim_opts globber_opts;

#endif
