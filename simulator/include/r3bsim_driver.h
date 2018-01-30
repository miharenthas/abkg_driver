//This header contains the definition of the simulation driver (derived from r3ball.C)
//and some helper functions, alongside some datatypes that should keep the code
//more agile and clean.

//STL includes
#include <map>
#include <string>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>

//ROOT includes
#include "TSystem.h"
#include "TStopwatch.h"
#include "TVirtualMC.h"
#include "TGeant3.h"
#include "TString.h"

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
#include "R3BCalifa.h"
#include "R3BTra.h"
#include "R3BStartrack.h"
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

//the parsers and the primer
#include "r3b_ascii_parsecfg.h"
#include "r3bsim_primer.h"

//customised ASCII generator
#include "r3b_ascii_gen.h"
#include "r3b_ascii_parsecfg.h"

//maximum number of steps to be passed to the monte carlo generator
#define _R3BSIM_MAX_N_STEP -15000

//------------------------------------------------------------------------------------
//the driver itself.
//the only argument, so, contains all the settings necessary to
//start the simulation
float r3bsim_driver( r3bsim_opts &so ); //so: Simulation Options
void r3bsim_geomant( FairRunSim *run, r3bsim_opts &so ); //switches the geometry
FairField *r3bsim_magmant( FairRunSim *run, r3bsim_opts &so ); //constructs the magnetic field


