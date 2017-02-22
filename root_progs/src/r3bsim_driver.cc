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
//allocate the options and init them to their defaults
r3bsim_opts *r3bsim_options_alloc(){
	r3bsim_opts *so = new r3bsim_opts;
	
	//notice that the default options are not
	//viable because of the NULL fDetList.
	so->nEvents = 1000000;
	so->fDetlist["null"] = "null";
	strcpy( so->Target, "LeadTarget" );
	so->fVis = kFALSE;
	strcpy( so->fMC, "TGeant3" );
	so->fUserPList = kFALSE;
	so->fR3BMagnet = kTRUE;
	so->fMeasCurrent = 2000.;
	strcpy( so->OutFile, "r3bsim_bkg.root" );
	strcpy( so->ParFile, "r3bpar_bkg.root" );
	so->EventFile = NULL;
	strcpy( so->usr_cuts, "./ugly/SetCuts.C" );
	
	return so;
}

//------------------------------------------------------------------------------------
//edit the options
//NOTE: despite the full cpying infrastructure of std::map, it cannot be handled
//      by va_arg because it's not trivially copiable. This means that a pointer to it
//      must be passed. This is stressed by the suffix "_PTR" in the enum
//TODO: this is vulnearble to buffer overflows. Add checks. 
void r3bsim_options_edit( r3bsim_opts *so, const r3bsim_fmt *format, ... ){
	//count the arguments
	int n_arg;
	for( n_arg=0; format[n_arg] != END_OF_FORMAT; ++n_arg );
	
	//init the argument list with the proper size
	va_list args; va_start( args, n_arg );
	
	for( int i=0; format[i] != END_OF_FORMAT; ++i ){
		switch( format[i] ){
			case NB_EVENTS :
				so->nEvents = va_arg( args, Int_t );
				break;
			case DET_LIST_PTR :
				typedef std::map<std::string, std::string> Detlist_t;
				so->fDetlist = *va_arg( args, Detlist_t* );
				break;
			case TARGET :
				strcpy( so->Target,  va_arg( args, char* ) );
				if( !strstr( so->fDetlist["TARGET"].c_str(), so->Target ) ){
					if( strstr( so->Target, "LeadTarget" ) )
						so->fDetlist["TARGET"] = "target_LeadTarget.geo.root";
					if( strstr( so->Target, "Para" ) )
						so->fDetlist["TARGET"] = "target_Para.geo.root";
					if( strstr( so->Target, "Para45" ) )
						so->fDetlist["TARGET"] = "target_Para45.geo.root";
					if( strstr( so->Target, "LiH" ) )
						so->fDetlist["TARGET"] = "target_LiH.geo.root";
				}
				break;
			case VISUALIZATION_READY :
				so->fVis = va_arg( args, int );
				break;
			case MC_ENGINE :
				fprintf( stderr, "r3bsim_options_edit: WARNING, at the moment only GEANT3 is available for this program. Using that instead of %s.\n", va_arg( args, char* ) );
				strcpy( so->fMC, "TGeant3" );
				break;
			case USER_PLIST :
				so->fUserPList = va_arg( args, int );
				break;
			case HAS_MAGNET :
				so->fR3BMagnet = va_arg( args, int );
				break;
			case MEASURE_CURRENT :
				so->fMeasCurrent = va_arg( args, Double_t );
				break;
			case OUT_FILE :
				strcpy( so->OutFile, va_arg( args, char* ) );
				break;
			case PAR_FILE :
				strcpy( so->ParFile, va_arg( args, char* ) );
				break;
			case EVENT_FILE :
				so->EventFile = va_arg( args, FILE* );
				break;
			case VERBOSE :
				so->verbose = va_arg( args, int );
				break;
			case USR_CUTS :
				strcpy( so->usr_cuts, va_arg( args, char* ) );
				break;
		}
	}
}

//------------------------------------------------------------------------------------
//free the options
void r3bsim_options_free( r3bsim_opts *so ){
	delete so;
}

//------------------------------------------------------------------------------------
//very ugly detector list parser
std::map<std::string, std::string> r3bsim_detmant( const char *det_opts ){	
	std::map<std::string, std::string> m;

	if( strstr( det_opts, ":TARGET:" ) ){
		if( strstr( det_opts, "LeadTarget" ) ) m["TARGET"] = "target_LeadTarget.geo.root";
		if( strstr( det_opts, "Para" ) ) m["TARGET"] = "target_Para.geo.root";
		if( strstr( det_opts, "Para45" ) ) m["TARGET"] = "target_Para45.geo.root";
		if( strstr( det_opts, "LiH" ) ) m["TARGET"] = "target_LiH.geo.root";
	}
	if( strstr( det_opts, ":TARGETWHEEL:" ) ) m["TARGETWHEEL"] = "no_file_needed";
	if( strstr( det_opts, ":TARGETSHIELDING:" ) ) m["TARGETSHIELDING"] = "no_file_needed";
	if( strstr( det_opts, ":TARGETATMOSPHERE:" ) ){
		if( strstr( det_opts, "AtmoVacuum" ) ) m["TARGETATMOSPHERE"] = "vacuum";
		if( strstr( det_opts, "AtmoAir" ) ) m["TARGETATMOSPHERE"] = "air";
	}
	if( strstr( det_opts, ":ALADIN:" ) ) m["ALADIN"] = "aladin_v13a.geo.root";
	if( strstr( det_opts, ":GLAD:" ) ) m["GLAD"] = "glad_v13a.geo.root";
	if( strstr( det_opts, ":CRYSTALBALL:" ) ) m["CRYSTALBALL"] = "cal_v13a.geo.root";
	if( strstr( det_opts, ":CALIFAv14a:" ) ||
	    strstr( det_opts, ":CALIFA:" ) ) m["CALIFA"] = "califa_v14a.geo.root";
	if( strstr( det_opts, ":CALIFAv13_811:" ) ) m["CALIFA"] = "califa_v13_811.geo.root";
	if( strstr( det_opts, ":TOF:" ) ) m["TOF"] = "tof_v13a.geo.root";
	if( strstr( det_opts, ":MTOF:" ) ) m["MTOF"] = "mtof_v13a.geo.root";
	if( strstr( det_opts, ":DTOF:" ) ) m["DTOF"] = "dtof_v15a.geo.root";
	if( strstr( det_opts, ":DHC:" ) ) m["DHC"] = "dch_v13a.geo.root";
	if( strstr( det_opts, ":TRACKER:" ) ) m["TRACKER"] = "tra_v13vac.geo.root";
	if( strstr( det_opts, ":STaRTrackv14a:" ) ||
	    strstr( det_opts, ":STaRTrack:" ) ) m["STaRTrack"] = "startra_v14a.geo.root";
	if( strstr( det_opts, ":STaRTrackv13a:" ) ) m["STaRTrack"] = "startra_v13a.geo.root";
	if( strstr( det_opts, ":GFI:" ) ) m["GFI"] = "gfi_v13a.geo.root";
	if( strstr( det_opts, ":LAND:" ) ) m["LAND"] = "land_v12a_10m.geo.root";
	if( strstr( det_opts, ":SCINTNEULAND:" ) ) m["SCINTNEULAND"] = "neuland_v12a_14m.geo.root";
	if( strstr( det_opts, ":VACVESSELCOOLv14a:" ) ||
	    strstr( det_opts, ":VACVESSELCOOL:" ) ) m["VACVESSELCOOL"] = "vacvessel_v14a.geo.root";
	if( strstr( det_opts, ":VACVESSELCOOLv13a:" ) ) m["VACVESSELCOOL"] = "vacvessel_v13a.geo.root";
	if( strstr( det_opts, ":MFI:" ) ) m["MFI"] = "mfi_v13a.geo.root";
	if( strstr( det_opts, ":PSP:" ) ) m["PSP"] = "psp_v13a.geo.root";
	if( strstr( det_opts, ":RATTLEPLANE:" ) ) m["RATTLEPLANE"] = "no_file_needed";
	//TODO: add the parsing of the position.
	
	return m;
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
	run->SetMaterials( "media_r3b.geo" );

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
	run->Init();

	//-------------------------------------------------
	//save the parameter file
	R3BFieldPar* fieldPar = (R3BFieldPar*) rtdb->getContainer("R3BFieldPar");
	if( !so.fDetlist["ALADIN"].empty() 
	    || !so.fDetlist["GLAD"].empty() )
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
	Int_t fieldScale = 1;

	//NB: <D.B>
	// If the Global Position of the Magnet is changed
	// the Field Map has to be transformed accordingly
	FairField *magField = NULL;
	if ( !so.fDetlist["ALADIN"].empty() ) {
		magField = new R3BAladinFieldMap( "AladinMaps" );
		((R3BAladinFieldMap*)magField)->SetCurrent( so.fMeasCurrent );
		((R3BAladinFieldMap*)magField)->SetScale( fieldScale );
		
		if ( so.fR3BMagnet == kTRUE ) {
			run->SetField( magField );
		} else {
			run->SetField( NULL );
		}
	} else if( !so.fDetlist["GLAD"].empty() ){
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
		R3BDetector* tra = new R3BTra("Tracker", kTRUE);
		tra->SetGeometryFileName( so.fDetlist["TRACKER"].c_str() );
		tra->SetEnergyCut(1e-4);
		run->AddModule(tra);
	}

	//R3B Target definition
	if( !so.fDetlist["TARGET"].empty() ) {
		R3BModule* target= new R3BTarget( so.Target );
		target->SetGeometryFileName( so.fDetlist["TARGET"].c_str() );
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
		R3BModule* vesselcool= new R3BVacVesselCool( so.Target );
		vesselcool->SetGeometryFileName( so.fDetlist["VACVESSELCOOL"].c_str() );
		run->AddModule(vesselcool);
	}

	//R3B Magnet definition
	if( !so.fDetlist["ALADIN"].empty() ) {
		R3BModule* mag = new R3BMagnet("AladinMagnet");
		mag->SetGeometryFileName( so.fDetlist["ALADIN"].c_str() );
		run->AddModule(mag);
	}

	//R3B Magnet definition
	if( !so.fDetlist["GLAD"].empty() ) {
		R3BModule* mag = new R3BGladMagnet("GladMagnet");
		mag->SetGeometryFileName( so.fDetlist["GLAD"].c_str() );
		run->AddModule(mag);
	}

	//R3B Crystal Calorimeter
	if( !so.fDetlist["CRYSTALBALL"].empty() ) {
		R3BDetector* xball = new R3BXBall("XBall", kTRUE);
		((R3BXBall*)xball)->SelectCollectionOption( 2 ); //causing the output of data
		                                                 //at all levels:
		                                                 // 0: default, only "DHIT"-ish
		                                                 // 1: only "HIT"-ish
		                                                 // 2: both.
		xball->SetGeometryFileName( so.fDetlist["CRYSTALBALL"].c_str() );
		run->AddModule(xball);
	}

	if( !so.fDetlist["CALIFA"].empty() ) {
		R3BDetector* calo = new R3BCalo("Califa", kTRUE);
		((R3BCalo *)calo)->SelectGeometryVersion(10);
		//Selecting the Non-uniformity of the crystals (1 means +-1% max deviation)
		((R3BCalo *)calo)->SetNonUniformity(1.0);
		calo->SetGeometryFileName( so.fDetlist["CALIFA"].c_str() );
		run->AddModule(calo);
	}

	// STaRTrack
	if( !so.fDetlist["STaRTrack"].empty()  ) {
		R3BDetector* tra = new R3BSTaRTra("STaRTrack", kTRUE);
		tra->SetGeometryFileName( so.fDetlist["STaRTrack"].c_str() );
	run->AddModule(tra);
		}

	// DCH drift chambers
	if( !so.fDetlist["DCH"].empty() ) {
		R3BDetector* dch = new R3BDch("Dch", kTRUE);
		dch->SetGeometryFileName( so.fDetlist["DCH"].c_str() );
		run->AddModule(dch);
	}

	// Tof
	if( !so.fDetlist["TOF"].empty() ) {
		R3BDetector* tof = new R3BTof("Tof", kTRUE);
		tof->SetGeometryFileName( so.fDetlist["TOF"].c_str() );
		run->AddModule(tof);
	}

	// mTof
	if( !so.fDetlist["MTOF"].empty() ) {
		R3BDetector* mTof = new R3BmTof("mTof", kTRUE);
		mTof->SetGeometryFileName( so.fDetlist["MTOF"].c_str() );
		run->AddModule(mTof);
	}

	// dTof
	if( !so.fDetlist["DTOF"].empty() ) {
		R3BDetector* dTof = new R3BdTof("dTof", kTRUE);
		dTof->SetGeometryFileName( so.fDetlist["DTOF"].c_str() );
		run->AddModule(dTof);
	}

	// GFI detector
	if( !so.fDetlist["GFI"].empty() ) {
		R3BDetector* gfi = new R3BGfi("Gfi", kTRUE);
		gfi->SetGeometryFileName( so.fDetlist["GFI"].c_str() );
		run->AddModule(gfi);
	}

	// Land Detector
	if( !so.fDetlist["LAND"].empty() ) {
		R3BDetector* land = new R3BLand("Land", kTRUE);
		land->SetVerboseLevel(1);
		land->SetGeometryFileName( so.fDetlist["LAND"].c_str() );
		run->AddModule(land);
	}

	// DEPRECATED: NeuLand Scintillator Detector
	if( !so.fDetlist["SCINTNEULAND"].empty() ){
		R3BDetector* land = new R3BLand("Land", kTRUE);
		land->SetVerboseLevel(1);
		land->SetGeometryFileName( so.fDetlist["SCINTNEULAND"].c_str() );
		run->AddModule(land);
	}

	// Neuland Detector
	if( !so.fDetlist["NEULAND"].empty() ){
		R3BDetector* neuland = new R3BNeuland();
		neuland->SetGeometryFileName( so.fDetlist["NEULAND"].c_str() );
		run->AddModule(neuland);
	}

	// MFI Detector
	if( !so.fDetlist["MFI"].empty() ){
		R3BDetector* mfi = new R3BMfi("Mfi", kTRUE);
		mfi->SetGeometryFileName( so.fDetlist["MFI"].c_str() );
		run->AddModule(mfi);
	}

	// PSP Detector
	if( !so.fDetlist["PSP"].empty() ){
		R3BDetector* psp = new R3BPsp("Psp", kTRUE);
		psp->SetGeometryFileName( so.fDetlist["PSP"].c_str() );
		run->AddModule(psp);
	}

	// Luminosity detector
	if( !so.fDetlist["LUMON"].empty() ) {
		R3BDetector* lumon = new ELILuMon("LuMon", kTRUE);
		lumon->SetGeometryFileName( so.fDetlist["LUMON"].c_str() );
		run->AddModule(lumon);
	}
	
	//The rattleplane
	if( !so.fDetlist["RATTLEPLANE"].empty() ) {
		R3BRattlePlane::rp_trf trf = {0, 0, 0, 0, 0, 0};
		R3BDetector* rattleplane = new R3BRattlePlane( trf, "rattleplane", true );
		run->AddModule( rattleplane );
	}
}
