//implementation of r3bsim_driver
#include "r3bsim_driver.h"

//------------------------------------------------------------------------------------
//Helper function that sets the environment up
void r3bsim_env_setup(){
	//get the home directory of R3BRoot
	TString dir = getenv("VMCWORKDIR");
	
	//sets an environment variable pointing to the
	//geometry directory.
	TString r3b_geomdir = dir + "/geometry";
	gSystem->Setenv("GEOMPATH",r3b_geomdir.Data()); //gSystem is a ROOT global variable...

	//Sets an enviromnent variable pointin
	//to the configuration directory.
	TString r3b_confdir = dir + "gconfig";
	gSystem->Setenv("CONFIG_DIR",r3b_confdir.Data());
}

//------------------------------------------------------------------------------------
//the simulation driver itself (from r3ball.C)
float r3bsim_driver( r3bsim_opts &so ){
	//timer start
	TStopwatch timer;
	timer.Start();

	//-------------------------------------------------
	//shut up the logger
	FairLogger *logger = FairLogger::GetLogger();
	logger->SetLogVerbosityLevel("LOW");
	if( so.verbose ) logger->SetLogScreenLevel("INFO");
	else logger->SetLogScreenLevel("ERROR");

	//-------------------------------------------------
	//allocate the Fair simulation runner
	FairRunSim *run = new FairRunSim();
	run->SetName( so.fMC );
	run->SetOutputFile( so.OutFile );
	FairRuntimeDb *rtdb = run->GetRuntimeDb();

	//-------------------------------------------------
	//associate the material
	run->SetMaterials( globber_opts.material_file );

	//make the geometry.
	r3bsim_geomant( run, so );

	//make magnetic field
	FairField *magField = r3bsim_magmant( run, so );

	//-------------------------------------------------
	//create the FAIR prime generator, fpg
	FairPrimaryGenerator *fpg = new FairPrimaryGenerator();

	//create the (cutsomised) ASCII generator, agn
	//and add it to the fair primary generator.
	r3b_ascii_gen *agn = new r3b_ascii_gen( so.verbose );
	//load the events in memory and update the number of
	//events if more than available specified
	unsigned int nb_evt = agn->slurp( so.EventFile, so.nEvents );
	if( nb_evt < so.nEvents ) so.nEvents = nb_evt;
	
	//add the generator
	fpg->AddGenerator( agn );
	run->SetGenerator( fpg );
	
	//-------------------------------------------------
	//retrieve the cut settings for the program and
	//pass them to the fair runner
	//NOTE: this is ugly and very bad practice and will
	//      be eliminated as soon as possible.
	//NOTE: since someone had the brilliant idea to
	//      hardcode some paths in FairRunSim, this is
	//      not only ugly, but doesn't work and you *MUST*
	//      modify the SetCuts.C in R3Broot/gconfig.
	//      A decent copy is provided in ./ugly.
	//      Which is even uglier.
	//TODO: work around this crud.
	//run->SetUserCuts( so.usr_cuts );
	
	//-------------------------------------------------
	//set the visualizations flag
	run->SetStoreTraj( so.fVis );

	//-------------------------------------------------
	//initialize the runner as set
	r3bsim_primer pri( globber_g3, globber_cuts, globber_opts );
	run->SetSimSetup( pri );
	run->Init();

	//-------------------------------------------------
	//save the parameter file
	R3BFieldPar* fieldPar = (R3BFieldPar*) rtdb->getContainer("R3BFieldPar");
	if( !so.fDetlist["ALAFIELD"].empty() 
	    || !so.fDetlist["GLAFIELD"].empty() )
	{
		fieldPar->SetParameters( magField );
		fieldPar->setChanged();
	}

	FairParRootFileIo *parOut = new FairParRootFileIo( kTRUE );
	parOut->open( so.ParFile );
	rtdb->setOutput( parOut );
	rtdb->saveOutput();
	if( so.verbose ) rtdb->print();
	
	//-------------------------------------------------
	//before run, set some gMC stuff
	if( !gMC ){ //check if the thing actually exists
		fprintf( stderr, "r3bsim_driver: gMC not set: that's fatal.\n" );
		exit( 1 );
	}
	//set the number of steps
	gMC->SetMaxNStep( _R3BSIM_MAX_N_STEP );

	//set verbosity for geant3
	((TGeant3*)gMC)->Gcflag()->itest = (int)so.verbose; //set GTRIG output
	((TGeant3*)gMC)->Gcflag()->iswit[3] = (int)so.verbose; //set ROOT_GTREV output
	
	//-------------------------------------------------
	//let's go
	run->Run( so.nEvents );

	//stop the timer
	timer.Stop();
	float wall_clock = timer.RealTime();

	//cleanup
	//NOTE: apparently, FairRunSim cannot be deleted without horrible consequences
	//      this also means that more than one simulation cannot be run within
	//      the same program (and the memory /cannot/ be freed). Well done, really.
	
	return wall_clock;
}
	
//------------------------------------------------------------------------------------
//magnetic field building helper function
FairField *r3bsim_magmant( FairRunSim *run, r3bsim_opts &so ){
	float fieldScale = so.field_scale; //apparently we need a minus

	//NB: <D.B>
	// If the Global Position of the Magnet is changed
	// the Field Map has to be transformed accordingly
	FairField *magField = NULL;
	if ( !so.fDetlist["ALAFIELD"].empty() ) {
		magField = new R3BAladinFieldMap( "AladinMaps" );
		((R3BAladinFieldMap*)magField)->SetCurrent( so.fMeasCurrent );
		((R3BAladinFieldMap*)magField)->SetScale( fieldScale );
		
		if ( so.fR3BMagnet == kTRUE ) {
			run->SetField( magField );
		} else {
			run->SetField( NULL );
		}
	} else if( !so.fDetlist["GLAFIELD"].empty() ){
		magField = new R3BGladFieldMap( "R3BGladMap" );
		((R3BGladFieldMap*)magField)->SetPosition( 0., 0., +350-119.94 );
		((R3BGladFieldMap*)magField)->SetScale( fieldScale );

		if ( so.fR3BMagnet == kTRUE ) {
			run->SetField( magField );
		} else {
			run->SetField( NULL );
		}
	}
	
	return magField;
}

//------------------------------------------------------------------------------------
//helper function to build the geometry  (straight from r3ball.C)
void r3bsim_geomant( FairRunSim *run, r3bsim_opts &so ){
	//R3B Cave definition
	FairModule* cave= new R3BCave( "CAVE" );
	cave->SetGeometryFileName( "r3b_cave.geo" );
	run->AddModule( cave );

	//Tracker
	//NOTE: this is here because TargetAtmosphere will check on it
	//      and make its geometry only if this hasn't been
	//      instantiated yet. This avoids overlaps.
	if( !so.fDetlist["TRACKER"].empty() ) {
		R3BDetector* tra = new R3BTra( so.fDetlist["TRACKER"].c_str() );
		tra->SetEnergyCut(1e-4);
		run->AddModule(tra);
	}

	//R3B Target definition
	if( !so.fDetlist["TARGET"].empty() ) {
		R3BModule* target= new R3BTarget( "gerald",
			TString( so.fDetlist["TARGET"].c_str() ) );
		run->AddModule(target);
	}
	
	//R3B target atmosphere
	if( !so.fDetlist["TARGETATMOSPHERE"].empty() ) {
		R3BTargetAtmosphere* target_atm;
		if( !so.fDetlist["TARGETATMOSPHERE"].compare( "air" ) ){
			target_atm = new R3BTargetAtmosphere( "target_atmosphere",
			                                      "R3BTargetAtmosphere",
			                                      R3BTargetAtmosphere::AIR );
		} else {
			target_atm = new R3BTargetAtmosphere( "target_atmosphere",
			                                      "R3BTargetAtmosphere",
			                                      R3BTargetAtmosphere::VACUUM );
		}
		run->AddModule(target_atm);
	}
	
	
	//R3B target wheel mountings
	if( !so.fDetlist["TARGETWHEEL"].empty() ) {
		R3BModule* target_wheel = new R3BTargetWheel( "target_wheel" );
		run->AddModule(target_wheel);
	}
	
	//R3B target shielding
	if( !so.fDetlist["TARGETSHIELDING"].empty() ) {
		R3BModule* target_shielding = new R3BTargetShielding( "target_shielding" );
		run->AddModule(target_shielding);
	}

	//R3B SiTracker Cooling definition
	if( !so.fDetlist["VACVESSELCOOL"].empty() ) {
		R3BModule* vesselcool= new R3BVacVesselCool( "bobby", 
			TString( so.fDetlist["VACVESSELCOOL"].c_str() ) );
		run->AddModule(vesselcool);
	}

	//R3B Magnet definition
	if( !so.fDetlist["ALADIN"].empty() ) {
		R3BModule* mag = new R3BMagnet( so.fDetlist["ALADIN"].c_str() );
		run->AddModule(mag);
	}

	//R3B Magnet definition
	if( !so.fDetlist["GLAD"].empty() ) {
		R3BModule* mag = new R3BGladMagnet( so.fDetlist["GLAD"].c_str() );
		run->AddModule(mag);
	}

	//R3B Crystal Calorimeter
	if( !so.fDetlist["CRYSTALBALL"].empty() ) {
		R3BDetector* xball = new R3BXBall( so.fDetlist["CRYSTALBALL"].c_str() );
		((R3BXBall*)xball)->SelectCollectionOption( 2 ); //causing the output of data
		                                                 //at all levels:
		                                                 // 0: default, only "DHIT"-ish
		                                                 // 1: only "HIT"-ish
		                                                 // 2: both.
		run->AddModule(xball);
	}

	if( !so.fDetlist["CALIFA"].empty() ) {
		R3BDetector* calo = new R3BCalifa( so.fDetlist["CALIFA"].c_str() );
		((R3BCalifa *)calo)->SelectGeometryVersion(10);
		//Selecting the Non-uniformity of the crystals (1 means +-1% max deviation)
		((R3BCalifa *)calo)->SetNonUniformity(1.0);
		run->AddModule(calo);
	}

	// STaRTrack
	if( !so.fDetlist["STaRTrack"].empty()  ) {
		R3BDetector* tra = new R3BStartrack( so.fDetlist["STaRTrack"].c_str() );
		run->AddModule(tra);
		}

	// DCH drift chambers
	if( !so.fDetlist["DCH"].empty() ) {
		R3BDetector* dch = new R3BDch( so.fDetlist["DCH"].c_str() );
		run->AddModule(dch);
	}

	// Tof
	if( !so.fDetlist["TOF"].empty() ) {
		R3BDetector* tof = new R3BTof( so.fDetlist["TOF"].c_str() );
		run->AddModule(tof);
	}

	// mTof
	if( !so.fDetlist["MTOF"].empty() ) {
		R3BDetector* mTof = new R3BmTof( so.fDetlist["MTOF"].c_str() );
		run->AddModule(mTof);
	}

	// dTof
	if( !so.fDetlist["DTOF"].empty() ) {
		R3BDetector* dTof = new R3BdTof( so.fDetlist["DTOF"].c_str() );
		run->AddModule(dTof);
	}

	// GFI detector
	if( !so.fDetlist["GFI"].empty() ) {
		R3BDetector* gfi = new R3BGfi( so.fDetlist["GFI"].c_str() );
		run->AddModule(gfi);
	}

	// Land Detector
	if( !so.fDetlist["LAND"].empty() ) {
		R3BDetector* land = new R3BLand( so.fDetlist["LAND"].c_str() );
		land->SetVerboseLevel(1);
		run->AddModule(land);
	}

	// DEPRECATED: NeuLand Scintillator Detector
	if( !so.fDetlist["SCINTNEULAND"].empty() ){
		R3BDetector* land = new R3BLand( so.fDetlist["SCINTNEULAND"].c_str() );
		land->SetVerboseLevel(1);
		run->AddModule(land);
	}

	// Neuland Detector
	if( !so.fDetlist["NEULAND"].empty() ){
		R3BDetector* neuland = new R3BNeuland( so.fDetlist["NEULAND"].c_str() );
		run->AddModule(neuland);
	}

	// MFI Detector
	if( !so.fDetlist["MFI"].empty() ){
		R3BDetector* mfi = new R3BMfi( so.fDetlist["MFI"].c_str() );
		run->AddModule(mfi);
	}

	// PSP Detector
	if( !so.fDetlist["PSP"].empty() ){
		R3BDetector* psp = new R3BPsp( so.fDetlist["PSP"].c_str() );
		run->AddModule(psp);
	}

	// Luminosity detector
	if( !so.fDetlist["LUMON"].empty() ) {
		R3BDetector* lumon = new ELILuMon( so.fDetlist["LUMON"].c_str() );
		run->AddModule(lumon);
	}
	
	//The rattleplane
	if( !so.fDetlist["RATTLEPLANE"].empty() ) {
		//seed a random sequence,
		//useful for unique name generation.
		R3BRattlePlane::seed_unique_namer();
		R3BRattlePlane::rp_specs specs = {0, 0, 0, 0, 0, 0, 30, 30, 5,
		                                  (R3BTAM_switcher)L'G'};
		char rattlespecs[1024];
		strcpy( rattlespecs, so.fDetlist["RATTLEPLANE"].c_str() );
		char *spc_tok = strtok( rattlespecs, "+" ); //pre-load:
		                                            //note that now spc_tok is "RattleSpecs"
		char rattlename[64];
		R3BRattlePlane *rattleplane;
		
		//instantiate as many rattleplanes as found in the specifications
		//and name them uniquely with the tool in the class.
		while( spc_tok != NULL ){
			spc_tok = strtok( NULL, "+" );
			if( spc_tok == NULL ) break; //leave if there aren't any specs
			sscanf( spc_tok,
				"[%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lc]",
				&specs.rot_x,
				&specs.rot_y,
				&specs.rot_z,
				&specs.T_x,
				&specs.T_y,
				&specs.T_z,
				&specs.width,
				&specs.height,
				&specs.depth,
				&specs.stuff ); //it's a wide character...

			strcpy( rattlename, "rattleplane_" );
			R3BRattlePlane::mk_unique_name( rattlename );
			rattleplane = new R3BRattlePlane( specs, rattlename, kTRUE );
			run->AddModule( rattleplane );
		}
	}

	//The stopper plane
	if( !so.fDetlist["STOPPERPLANE"].empty() ) {
		//seed a random sequence,
		//useful for unique name generation.
		R3BRattlePlane::seed_unique_namer();
		R3BRattlePlane::rp_specs specs = {0, 0, 0, 0, 0, 0, 30, 30, 10,
		                                  (R3BTAM_switcher)L'G'};
		char stopperspecs[1024];
		strcpy( stopperspecs, so.fDetlist["STOPPERPLANE"].c_str() );
		char *spc_tok = strtok( stopperspecs, "+" ); //pre-load:
		                                            //note that now spc_tok is "RattleSpecs"
		char stoppername[64];
		R3BStopperPlane *stopperplane;
		
		//instantiate as many rattleplanes as found in the specifications
		//and name them uniquely with the tool in the class.
		while( spc_tok != NULL ){
			spc_tok = strtok( NULL, "+" );
			if( spc_tok == NULL ) break; //leave if there aren't any specs
			sscanf( spc_tok,
				"[%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf]",
				&specs.rot_x,
				&specs.rot_y,
				&specs.rot_z,
				&specs.T_x,
				&specs.T_y,
				&specs.T_z,
				&specs.width,
				&specs.height );
			specs.depth = 10; //fixing it here
			specs.stuff = (R3BTAM_switcher)L'G'; //always lead --want
			                                     //them to interact ASAP

			strcpy( stoppername, "stopperplane_" );
			R3BRattlePlane::mk_unique_name( stoppername );
			stopperplane = new R3BStopperPlane( specs, stoppername, kTRUE );
			run->AddModule( stopperplane );
		}
	}
}
