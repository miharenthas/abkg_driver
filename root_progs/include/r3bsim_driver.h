//This header contains the definition of the simulation driver (derived from r3ball.C)
//and some helper functions, alongside some datatypes that should keep the code
//more agile and clean.

//STL includes
#include <map>
#include <string>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

//ROOT includes
#include "TSystem.h"
#include "TStopwatch.h"
#include "TVirtualMC.h"
#include "TGeant3.h"

//a very ugly check on ROOT version
//because the "machine independent types" are defined
//in a header only starting from ROOT 6.xx.xx
#include "RVersion.h"
#if ROOT_VERSION_CODE >= ROOT_VERSION( 5, 99, 99 )
	#include "RtypesCore.h"
#endif

//FAIR includes
#include "FairRunSim.h"
#include "FairModule.h"
#include "FairParRootFileIo.h"
#include "FairLogger.h"
#include "FairParRootFileIo.h"

//R3B includes
#include "R3BModule.h"
#include "R3BDetector.h"
#include "R3BTarget.h"
#include "R3BTargetAtmosphere.h"
#include "R3BTargetWheel.h"
#include "R3BTargetShielding.h"
#include "R3BCave.h"
#include "R3BVacVesselCool.h"
#include "R3BMagnet.h"
#include "R3BGladMagnet.h"
#include "R3BXBall.h"
#include "R3BCalo.h"
#include "R3BTra.h"
#include "R3BSTaRTra.h"
#include "R3BDch.h"
#include "R3BTof.h"
#include "R3BmTof.h"
#include "R3BdTof.h"
#include "R3BLand.h"
#include "R3BNeuland.h"
#include "R3BMfi.h"
#include "R3BPsp.h"
#include "R3BGfi.h"
#include "ELILuMon.h"
#include "R3BAladinFieldMap.h"
#include "R3BGladFieldMap.h"
#include "R3BFieldPar.h"
#include "R3BRattlePlane.h"
#include "R3BStopperPlane.h"

//customised ASCII generator
#include "r3b_ascii_gen.h"

//maximum number of steps to be passed to the monte carlo generator
#define _R3BSIM_MAX_N_STEP -15000

//------------------------------------------------------------------------------------
//Some handy data structure (just to keep the options in order
//and not having to send around humongous amount of parameters)
typedef struct r3bsim_background_generator_options {
	Int_t nEvents;
	std::map<std::string, std::string> fDetlist;
	char Target[128];
	Bool_t fVis;
	char fMC[128];
	Bool_t fUserPList;
	Bool_t fR3BMagnet;
	Double_t fMeasCurrent;
	char OutFile[128];
	char ParFile[128];
	FILE *EventFile;
	Bool_t verbose;
	char usr_cuts[128];
	float field_scale;
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
	FIELD_SCALE //sets the field scale, wide character N
} r3bsim_fmt;

//------------------------------------------------------------------------------------
//the driver itself.
//the only argument, so, contains all the settings necessary to
//start the simulation
float r3bsim_driver( r3bsim_opts &so ); //so: Simulation Options
void r3bsim_geomant( FairRunSim *run, r3bsim_opts &so ); //switches the geometry
FairField *r3bsim_magmant( FairRunSim *run, r3bsim_opts &so ); //constructs the magnetic field
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

NOTE: this implementation was the lowes hanging fruit.
      Probably smarter things are coming.
*************************************************************************************/
std::map<std::string, std::string> r3bsim_detmant( const char *det_opts ); //parse a detector description.
std::string r3bsim_make_geofname( const char *prefix, const char *tag_ptr ); //make a file name

//------------------------------------------------------------------------------------
//helper functions:
void r3bsim_env_setup(); //sets up the environment


r3bsim_opts *r3bsim_options_alloc(); //allocate the options with the default values
/*************************************************************************************
                                   NOTE ON THE FORMAT
//coming soon.
*************************************************************************************/
void r3bsim_options_edit( r3bsim_opts *so, const r3bsim_fmt *format, ... ); //edit the options
                                                                            //based on a format string
void r3bsim_options_free( r3bsim_opts *so ); //deallocate the options.

