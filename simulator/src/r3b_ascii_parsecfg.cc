//the implementation of r3b_ascii_parsecfg

#include "r3b_ascii_parsecfg.h"

//------------------------------------------------------------------------------------
//the three very very ugly global variables
r3bsim_g3setup globber_g3;
r3bsim_cuts globber_cuts;
r3bsim_opts globber_opts;

//------------------------------------------------------------------------------------
//utils previously found in r3bsim_driver

//------------------------------------------------------------------------------------
//allocate the options and init them to their defaults
void r3bsim_options_init(){
	//notice that the default options are not
	//viable because of the NULL fDetlist.
	globber_opts.nEvents = INT_MAX;
	globber_opts.fDetlist["null"] = "null";
	strcpy( globber_opts.Target, "LeadTarget" );
	globber_opts.fVis = false;
	strcpy( globber_opts.fMC, "TGeant3" );
	globber_opts.fUserPList = false;
	globber_opts.fR3BMagnet = true;
	globber_opts.fMeasCurrent = 2000.;
	strcpy( globber_opts.OutFile, "bkg.root" );
	strcpy( globber_opts.ParFile, "/dev/null" );
	globber_opts.EventFile = NULL;
	strcpy( globber_opts.usr_cuts, "./ugly/SetCuts.C" );
	globber_opts.field_scale = -0.5; //apparently, this is the default...
	strcpy( globber_opts.material_file, "media_r3b.geo" );
}

//------------------------------------------------------------------------------------
//edit the options
//NOTE: despite the full cpying infrastructure of std::map, it cannot be handled
//      by va_arg because it's not trivially copiable. This means that a pointer to it
//      must be passed. This is stressed by the suffix "_PTR" in the enum
//TODO: this is vulnearble to buffer overflows. Add checks. 
void r3bsim_options_edit( const r3bsim_fmt *format, ... ){
	//count the arguments
	int n_arg;
	for( n_arg=0; format[n_arg] != END_OF_FORMAT; ++n_arg );
	
	//init the argument list with the proper size
	va_list args; va_start( args, n_arg );
	
	for( int i=0; format[i] != END_OF_FORMAT; ++i ){
		switch( format[i] ){
			case NB_EVENTS :
				globber_opts.nEvents = va_arg( args, int );
				break;
			case DET_LIST_PTR :
				typedef std::map<std::string, std::string> Detlist_t;
				globber_opts.fDetlist = *va_arg( args, Detlist_t* );
				break;
			case TARGET :
				strcpy( globber_opts.Target,  va_arg( args, char* ) );
				if( !strstr( globber_opts.fDetlist["TARGET"].c_str(), globber_opts.Target ) ){
					if( strstr( globber_opts.Target, "LeadTarget" ) )
						globber_opts.fDetlist["TARGET"] = "target_LeadTarget.geo.root";
					if( strstr( globber_opts.Target, "Para" ) )
						globber_opts.fDetlist["TARGET"] = "target_Para.geo.root";
					if( strstr( globber_opts.Target, "Para45" ) )
						globber_opts.fDetlist["TARGET"] = "target_Para45.geo.root";
					if( strstr( globber_opts.Target, "LiH" ) )
						globber_opts.fDetlist["TARGET"] = "target_LiH.geo.root";
				}
				break;
			case VISUALIZATION_READY :
				globber_opts.fVis = va_arg( args, int );
				break;
			case MC_ENGINE :
				fprintf( stderr, "r3bsim_options_edit: WARNING, at the moment only GEANT3 is available for this program. Using that instead of %s.\n", va_arg( args, char* ) );
				strcpy( globber_opts.fMC, "TGeant3" );
				break;
			case USER_PLIST :
				globber_opts.fUserPList = va_arg( args, int );
				break;
			case HAS_MAGNET :
				globber_opts.fR3BMagnet = va_arg( args, int );
				break;
			case MEASURE_CURRENT :
				globber_opts.fMeasCurrent = va_arg( args, double );
				break;
			case OUT_FILE :
				strncpy( globber_opts.OutFile, va_arg( args, char* ), 128 );
				break;
			case PAR_FILE :
				strncpy( globber_opts.ParFile, va_arg( args, char* ), 128 );
				break;
			case EVENT_FILE :
				globber_opts.EventFile = va_arg( args, FILE* );
				break;
			case VERBOSE :
				globber_opts.verbose = va_arg( args, int );
				break;
			case USR_CUTS :
				strncpy( globber_opts.usr_cuts, va_arg( args, char* ), 128 );
				break;
			case FIELD_SCALE :
				globber_opts.field_scale = va_arg( args, double );
				break;
			case MATERIAL_FILE :
				strncpy( globber_opts.material_file, va_arg( args, char* ), 128 );
				break;
		}
	}
}

//------------------------------------------------------------------------------------
//make the geometry file name with an optional geometry tag.
//TODO: turn this in a tool which can actually build the file name
//      from a specified version and a given path!
std::string r3bsim_make_geofname( const std::string &spec ){
	//I still have to see the regularity in the file names here...
	return "/dev/null";
}

//------------------------------------------------------------------------------------
//very ugly detector list parser
//TODO: use the make_geofname util!
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
	if( strstr( det_opts, ":ALADIN:" ) ){
		m["ALADIN"] = "aladin_v13a.geo.root";
		m["ALAFIELD"] = "alafield";
	}
	if( strstr( det_opts, ":ALAFIELD:" ) ) m["ALAFIELD"] = "alafield";
	if( strstr( det_opts, ":GLAD:" ) ){
		if( strstr( det_opts, "GladVacuum" ) ) m["GLAD"] = "glad_v13a_vacuum.geo.root";
		else m["GLAD"] = "glad_v13a.geo.root";
		m["GLAFIELD"] = "glafield";
	}
	if( strstr( det_opts, ":GLAFIELD:" ) ) m["GLAFIELD"] = "glafield";
	if( strstr( det_opts, ":CRYSTALBALL:" ) ) m["CRYSTALBALL"] = "cal_v13a.geo.root";
	if( strstr( det_opts, ":CALIFAv14a:" ) ||
	    strstr( det_opts, ":CALIFA:" ) ) m["CALIFA"] = "califa_v14a.geo.root";
	if( strstr( det_opts, ":CALIFAv13_811:" ) ) m["CALIFA"] = "califa_v13_811.geo.root";
	if( strstr( det_opts, ":TOF:" ) ) m["TOF"] = "tof_v13a.geo.root";
	if( strstr( det_opts, ":MTOF:" ) ) m["MTOF"] = "mtof_v13a.geo.root";
	if( strstr( det_opts, ":DTOF:" ) ) m["DTOF"] = "dtof_v15a.geo.root";
	if( strstr( det_opts, ":DCH:" ) ) m["DCH"] = "dch_v13a.geo.root";
	if( strstr( det_opts, ":TRACKER:" ) ) m["TRACKER"] = "tra_v13vac.geo.root";
	if( strstr( det_opts, ":STaRTrackv15a:" ) ||
	    strstr( det_opts, ":STaRTrack:" ) ||
	    strstr( det_opts, ":Startrack:" ) ) m["STaRTrack"] = "startra_v15a.geo.root";
	if( strstr( det_opts, ":StaRTtrakv14a:" ) ) m["STaRTrack"] = "startra_v14a.geo.root";
	if( strstr( det_opts, ":STaRTrackv13a:" ) ) m["STaRTrack"] = "startra_v13a.geo.root";
	if( strstr( det_opts, ":GFI:" ) ) m["GFI"] = "gfi_v13a.geo.root";
	if( strstr( det_opts, ":LAND:" ) ) m["LAND"] = "land_v12a_10m.geo.root";
	if( strstr( det_opts, ":SCINTNEULAND:" ) ) m["SCINTNEULAND"] = "neuland_v12a_14m.geo.root";
	if( strstr( det_opts, ":VACVESSELCOOLv14a:" ) ||
	    strstr( det_opts, ":VACVESSELCOOL:" ) ) m["VACVESSELCOOL"] = "vacvessel_v14a.geo.root";
	if( strstr( det_opts, ":VACVESSELCOOLv13a:" ) ) m["VACVESSELCOOL"] = "vacvessel_v13a.geo.root";
	if( strstr( det_opts, ":MFI:" ) ) m["MFI"] = "mfi_v13a.geo.root";
	if( strstr( det_opts, ":PSP:" ) ) m["PSP"] = "psp_v13a.geo.root";
	
	//rattleplane section: requires a bit of attention because I'd like to put more
	//than one in (eventually, not yet implemented)
	char rp_specbuf[1024];
	const char *p_end, *p_begin;
	if( strstr( det_opts, ":RATTLEPLANE:" ) ){
		if( (p_begin = strstr( det_opts, "RattleSpecs" )) != NULL ){
			p_end = strstr( p_begin, "SpecEnd" );
			memcpy( rp_specbuf, p_begin, (p_end-p_begin)*sizeof(char) );
			m["RATTLEPLANE"] = rp_specbuf;
		} else m["RATTLEPLANE"] = "RattleSpecs+[0,0,0,0,0,0,30,30,5,G]";
	}

	//stopper plane section: it's a rattleplane, only kills the particles on entrance.
	if( strstr( det_opts, ":STOPPERPLANE:" ) ){
		if( (p_begin = strstr( det_opts, "StopperSpecs" )) != NULL ){
			p_end = strstr( p_begin, "SpecEnd" );
			memcpy( rp_specbuf, p_begin, (p_end-p_begin)*sizeof(char) );
			m["STOPPERPLANE"] = rp_specbuf;
		} else m["STOPPERPLANE"] = "StopperSpecs+[0,0,0,0,0,0,30,30]";
	}
	
	return m;
}

//------------------------------------------------------------------------------------
//the parsers

//------------------------------------------------------------------------------------
//widgets
//strip comment from a line
char *r3b_ascii_curtail( char *line, const char *cc ){
	for( const char *c = cc; *c != '\0'; ++c )
		for( char *p = line; *p != '\0'; ++p )
			if( *p == *c ){ *p = '\0'; break; }
	return line;
}

char *r3b_ascii_strip( char *line, const char *cc ){
	char found, *lp = line;
	for( char *p=line; *p != '\0'; ++p ){
		found = 0;
		for( const char *c=cc; *c != '\0'; ++c )
			if( *p == *c ) ++found;
		if( found ) continue;
		*lp = *p;
		++lp;
	}
	*lp = '\0';
	return line;
}

//break up a line into words
std::vector<std::string> r3b_ascii_segment( const char *line ){
	int ll = strlen( line );
	char *tok, *lb = (char*)malloc( ll+1 );
	strcpy( lb, line );
	tok = strtok( lb, " " );
	std::vector<std::string> words;
	while( tok ){ words.push_back( tok ); tok = strtok( NULL, " " ); }
	free( lb );
	return words;
}

//------------------------------------------------------------------------------------
//the top dog
int r3b_ascii_parse( FILE *stream ){
	char *line = (char*)calloc( 512, sizeof(char) );
	std::vector<std::string> words;
	std::string section;
	int records = 0;
	
	while( !feof( stream ) ){
		fgets( line, 512, stream );
		if( feof( stream ) ) break;
		++records;
		
		line = r3b_ascii_curtail( line, "#\n" );
		line = r3b_ascii_strip( line, "\t" );
		words = r3b_ascii_segment( line );
		if( !words.size() ) continue;
		
		int i=0;
		for( i; i < words.size(); ++i ){
			if( words[i] != "section" ){
				fprintf( stderr, "Line %d: %s is not a section.\n", records,
				         words[i].c_str() );
				exit( 101 );
			} else break;
		}
		if( words.size() == i+1 ){
			fprintf( stderr, "There's an unnamed section.\n" );
			exit( 102 );
		}
		section = words[i+1];
		words.clear();
		
		if( section == "detectors" )
			r3b_ascii_parse__detectors( stream, records );
		else if( section == "detector-list" )
			r3b_ascii_parse__dlist( stream, records );
		else if( section == "options" )
			r3b_ascii_parse__opts( stream, records );
		else if( section == "geant3" )
			r3b_ascii_parse__g3( stream, records );
		else if( section == "geant4" )
			r3b_ascii_parse__g4( stream, records );
		else if( section == "fluka" )
			r3b_ascii_parse__fluka( stream, records );
		else if( section == "cuts" )
			r3b_ascii_parse__cuts( stream, records );
		else{
			fprintf( stderr, "Line %d, %s is not a valid section name.\n", records,
			         section.c_str() );
			exit( 103 );
		}
	}
	
	free( line );
	return records;
}

//------------------------------------------------------------------------------------
//specialized parsers

//------------------------------------------------------------------------------------
//cuts
void r3b_ascii_parse__cuts( FILE *stream, int &records ){
	char *line = (char*)calloc( 512, sizeof(char) );
	std::vector<std::string> words;
	std::string field, value;
	
	while( !feof( stream ) ){
		fgets( line, 512, stream );
		if( feof( stream ) ){
			fprintf( stderr, "Line %d: Unexpected end-of-file.\n", records );
			exit( 104 );
		}
		++records;
		
		line = r3b_ascii_curtail( line, "#\n" );
		line = r3b_ascii_strip( line, "\t" );
		words = r3b_ascii_segment( line );
		if( !words.size() ) continue;
		else if( words[0] == "endsection" ) break;
		else if( words.size() != 2 ){
			fprintf( stderr, "Line %d: all cuts need a name and a value.\n", records );
			exit( 105 );
		}
		field = words[0];
		value = words[1];
		
		if( field == "gamma" )
			globber_cuts.gamma = atof( value.c_str() );
		else if( field == "electron" )
			globber_cuts.electron = atof( value.c_str() );
		else if( field == "neutral_had" )
			globber_cuts.neutral_had = atof( value.c_str() );
		else if( field == "charged_had" )
			globber_cuts.charged_had = atof( value.c_str() );
		else if( field == "muon" )
			globber_cuts.muon = atof( value.c_str() );
		else if( field == "electron_bremss" )
			globber_cuts.electron_bremss = atof( value.c_str() );
		else if( field == "muon_bremss" )
			globber_cuts.muon_bremss = atof( value.c_str() );
		else if( field == "electron_delta" )
			globber_cuts.electron_delta = atof( value.c_str() );
		else if( field == "muon_delta" )
			globber_cuts.muon_delta = atof( value.c_str() );
		else if( field == "muon_pair" )
			globber_cuts.muon_pair = atof( value.c_str() );
		else if( field == "max_ToF" )
			globber_cuts.max_ToF = atof( value.c_str() );
		else{
			fprintf( stderr, "Line %d: %d is not a valid cut name.\n", records,
			         field.c_str() );
		}
	}
	
	free( line );
}

void r3b_ascii_parse__g3( FILE *stream, int &records ){
	char *line = (char*)calloc( 512, sizeof(char) );
	std::vector<std::string> words;
	std::string field;
	
	while( !feof( stream ) ){
		fgets( line, 512, stream );
		if( feof( stream ) ){
			fprintf( stderr, "Line %d: Unexpected end-of-file.\n", records );
			exit( 104 );
		}
		++records;
		
		line = r3b_ascii_curtail( line, "#\n" );
		line = r3b_ascii_strip( line, "\t" );
		words = r3b_ascii_segment( line );
		if( !words.size() ) continue;
		else if( words[0] == "endsection" ) break;
		field = words[0];
		
#define CK_NB_WORDS( x ) if( words.size() != x+1 ){\
	fprintf( stderr, "Line %d, field %s needs %d arguments\n", records, field.c_str(), x );\
	exit( 106 );\
		}
		
		if( field == "trig" ){
			CK_NB_WORDS( 1 )
			globber_g3.trig = atoi( words[1].c_str() );
		} else if( field == "swit" ){
			CK_NB_WORDS( 2 )
			globber_g3.swit[0] = atoi( words[1].c_str() );
			globber_g3.swit[1] = atoi( words[2].c_str() );
		} else if( field == "debu" ) {
			CK_NB_WORDS( 3 )
			globber_g3.debu[0] = atoi( words[1].c_str() );
			globber_g3.debu[1] = atoi( words[2].c_str() );
			globber_g3.debu[2] = atoi( words[3].c_str() );
		} else if( field == "auto" ) {
			CK_NB_WORDS( 1 )
			globber_g3.gauto = atoi( words[1].c_str() );
		} else if( field == "aban" ) {
			CK_NB_WORDS( 1 )
			globber_g3.aban = atoi( words[1].c_str() );
		} else if( field == "opti" ) {
			CK_NB_WORDS( 1 )
			globber_g3.opti = atoi( words[1].c_str() );
		} else if( field == "eran" ) {
			CK_NB_WORDS( 1 )
			globber_g3.eran = atof( words[1].c_str() );
		} else if( field == "ckov" ) {
			CK_NB_WORDS( 1 )
			globber_g3.ckov = atoi( words[1].c_str() );
		} else if( field == "rayl" ) {
			CK_NB_WORDS( 1 )
			globber_g3.rayl = atoi( words[1].c_str() );
		} else if( field == "pair" ) {
			CK_NB_WORDS( 1 )
			globber_g3.pair = atoi( words[1].c_str() );
		} else if( field == "comp" ) {
			CK_NB_WORDS( 1 )
			globber_g3.comp = atoi( words[1].c_str() );
		} else if( field == "phot" ) {
			CK_NB_WORDS( 1 )
			globber_g3.phot = atoi( words[1].c_str() );
		} else if( field == "pfis" ) {
			CK_NB_WORDS( 1 )
			globber_g3.pfis = atoi( words[1].c_str() );
		} else if( field == "dray" ) {
			CK_NB_WORDS( 1 )
			globber_g3.dray = atoi( words[1].c_str() );
		} else if( field == "anni" ) {
			CK_NB_WORDS( 1 )
			globber_g3.anni = atoi( words[1].c_str() );
		} else if( field == "brem" ) {
			CK_NB_WORDS( 1 )
			globber_g3.brem = atoi( words[1].c_str() );
		} else if( field == "hadr" ) {
			CK_NB_WORDS( 1 )
			globber_g3.hadr = atoi( words[1].c_str() );
		} else if( field == "munu" ) {
			CK_NB_WORDS( 1 )
			globber_g3.munu = atoi( words[1].c_str() );
		} else if( field == "dcay" ) {
			CK_NB_WORDS( 1 )
			globber_g3.dcay = atoi( words[1].c_str() );
		} else if( field == "loss" ) {
			CK_NB_WORDS( 1 )
			globber_g3.loss = atoi( words[1].c_str() );
		} else if( field == "muls" ) {
			CK_NB_WORDS( 1 )
			globber_g3.muls = atoi( words[1].c_str() );
		} else if( field == "stra" ) {
			CK_NB_WORDS( 1 )
			globber_g3.stra = atoi( words[1].c_str() );
		} else if( field == "sync" ) {
			CK_NB_WORDS( 1 )
			globber_g3.sync = atoi( words[1].c_str() );
		} else {
			fprintf( stderr, "Line %d: %s is not a recognised Geant3 option.\n",
			         records, field.c_str() );
			exit( 107 );
		}
#undef CK_NB_WORDS
	}
	
	free( line );
}

//------------------------------------------------------------------------------------
//the detector parser thingie
void r3b_ascii_parse__detectors( FILE *stream, int &records ){
	char *line = (char*)calloc( 512, sizeof(char) );
	std::vector<std::string> words;
	std::string field, fname;
	
	while( !feof( stream ) ){
		fgets( line, 512, stream );
		if( feof( stream ) ){
			fprintf( stderr, "Line %d: Unexpected end-of-file.\n", records );
			exit( 104 );
		}
		++records;
		
		line = r3b_ascii_curtail( line, "#\n" );
		line = r3b_ascii_strip( line, "\t" );
		words = r3b_ascii_segment( line );
		if( !words.size() ) continue;
		else if( words[0] == "endsection" ) break;
		else if( words.size() != 2 ){
			fprintf( stderr, "Line %d: All detectors need a name and a specification.\n",
			         records );
			exit( 108 );
		}
		field = words[0];
		
		//TODO: make use of the make_geofname tool!!!
//		if( words[1][0] == '%' )
// 			fname = r3bsim_make_geofname( words[1] );
// 		else fname = words[1];
		fname = words[1];
		
		if( field == "TARGET" ) globber_opts.fDetlist["TARGET"] = fname;
		//NOTE: the targetwheel -shielding and -atmosphere don't actually use files
		//      so the string is copied but not used at the moment.
		else if( field == "TARGETWHEEL" ) globber_opts.fDetlist["TARGETWHEEL"] = fname;
		else if( field == "TARGETSHIELDING" ) globber_opts.fDetlist["TARGETSHIELDING"] = fname;
		else if( field == "TARGETATMOSPHERE" ) globber_opts.fDetlist["TARGETATMOSPHERE"] = fname;
		else if( field == "ALADIN" ) globber_opts.fDetlist["ALADIN"] = fname;
		else if( field == "GLAD" ) globber_opts.fDetlist["GLAD"] = fname;
		else if( field == "ALAFIELD" ) globber_opts.fDetlist["ALAFIELD"] = fname;
		else if( field == "GLAFIELD" ) globber_opts.fDetlist["GLAFIELD"] = fname;
		else if( field == "CRYSTALBALL" ) globber_opts.fDetlist["CRYSTALBALL"] = fname;
		else if( field == "CALIFA" ) globber_opts.fDetlist["CALIFA"] = fname;
		else if( field == "TOF" ) globber_opts.fDetlist["TOF"] = fname;
		else if( field == "MTOF" ) globber_opts.fDetlist["MTOF"] = fname;
		else if( field == "DTOF" ) globber_opts.fDetlist["DTOF"] = fname;
		else if( field == "DCH" ) globber_opts.fDetlist["DCH"] = fname;
		else if( field == "TRACKER" ) globber_opts.fDetlist["TRACKER"] = fname;
		else if( field == "STaRTrack" ) globber_opts.fDetlist["STaRTrack"] = fname;
		else if( field == "GFI" ) globber_opts.fDetlist["GFI"] = fname;
		else if( field == "LAND" ) globber_opts.fDetlist["LAND"] = fname;
		else if( field == "SCINTNEULAND" ) globber_opts.fDetlist["SCINTNEULAND"] = fname;
		else if( field == "VACVESSELCOOL" ) globber_opts.fDetlist["VACVESSELCOOL"] = fname;
		else if( field == "MFI" ) globber_opts.fDetlist["MFI"] = fname;
		else if( field == "PSP" ) globber_opts.fDetlist["PSP"] = fname;
		//NOTE: rattleplane and stopperplane don't use geometry files, but the string is
		//      supposed to contain specs, like "RattleSpecs+[rx,ry,rz,tx,ty,tz,h,w,d,mat]
		//      more on it on the docs.
		else if( field == "RATTLEPLANE" ) globber_opts.fDetlist["RATTLEPLANE"] = fname;
		else if( field == "STOPPERPLANE" ) globber_opts.fDetlist["STOPPERPLANE"] = fname;
		else{
			fprintf( stderr, "Line %d: %s is not a recognised detector name.\n",
			         records, field.c_str() );
			exit( 109 );
		}
	}
	
	free( line );
}

//------------------------------------------------------------------------------------
//parse (one or several) detector lists, with the old tool
void r3b_ascii_parse__dlist( FILE *stream, int &records ){
	char *line = (char*)calloc( 512, sizeof(char) );
	std::vector<std::string> words;
	std::map<std::string, std::string> m;
	
	while( !feof( stream ) ){
		fgets( line, 512, stream );
		if( feof( stream ) ){
			fprintf( stderr, "Line %d: Unexpected end-of-file.\n", records );
			exit( 104 );
		}
		++records;
		
		line = r3b_ascii_curtail( line, "#\n" );
		line = r3b_ascii_strip( line, "\t" );
		words = r3b_ascii_segment( line );
		if( words[0] == "endsection" ) break;
		
		for( int i=0; i < words.size(); ++i ){
			m = r3bsim_detmant( words[i].c_str() );
			globber_opts.fDetlist.insert( m.begin(), m.end() );
		}
	}
	
	free( line );
}

//------------------------------------------------------------------------------------
//the option parser
void r3b_ascii_parse__opts( FILE *stream, int &records ){
	char *line = (char*)calloc( 512, sizeof(char) );
	std::vector<std::string> words;
	std::string field, value;
	
	while( !feof( stream ) ){
		fgets( line, 512, stream );
		if( feof( stream ) ){
			fprintf( stderr, "Line %d: Unexpected end-of-file.\n", records );
			exit( 104 );
		}
		++records;
		
		line = r3b_ascii_curtail( line, "#\n" );
		line = r3b_ascii_strip( line, "\t" );
		words = r3b_ascii_segment( line );
		if( !words.size() ) continue;
		else if( words[0] == "endsection" ) break;
		else if( words.size() != 2 ){
			fprintf( stderr, "Line %d: all options need a name and a value.\n", records );
			exit( 110 );
		}
		field = words[0];
		value = words[1];
		
		if( field == "nb-events" ) globber_opts.nEvents = atoi( value.c_str() );
		else if( field == "save-geometry" ){
			if( value == "yes" || value == "y" || value == "Y" || value == "YES" || value == "Yes" ) globber_opts.fVis = true;
			else globber_opts.fVis = false;
		} else if( field == "MC-engine" ) strncpy( globber_opts.fMC, value.c_str(), 128 );
		else if( field == "physics-list" ); //do nothing, this option is disabled.
		else if( field == "use-magnet" ){
			if( value == "yes" || value == "y" || value == "Y" || value == "YES" || value == "Yes" ) globber_opts.fVis = true;
			else globber_opts.fVis = false;
		} else if( field == "magnet-current" ) globber_opts.fMeasCurrent = atof( value.c_str() );
		else if( field == "output-file" ) strncpy( globber_opts.OutFile, value.c_str(), 128 );
		else if( field == "par-file" ) strncpy( globber_opts.ParFile, value.c_str(), 128 );
		else if( field == "field-scale" ) globber_opts.field_scale = atof( value.c_str() );
		else if( field == "mat-file" ) strncpy( globber_opts.material_file, value.c_str(), 128 ); 
		else{
			fprintf( stderr, "Line %d: option %s is unknown.\n", records, field.c_str() );
			exit( 111 );
		}
	}
	
	free( line );
}

//------------------------------------------------------------------------------------
//geant 4 and fluka, TODO
void r3b_ascii_parse__g4( FILE *stream, int &records ){}
void r3b_ascii_parse__fluka( FILE *stream, int &records ){}

//------------------------------------------------------------------------------------
//dumpers, to print out the globa object (targets of the parsers)
void r3b_ascii_dump_opts( FILE *stream ){
	fputs( "#these are the options for this run\n", stream );
	fputs( "section options\n", stream );
	
	fprintf( stream, "\tnb-events %d\n", globber_opts.nEvents );
	if( globber_opts.fVis ) fprintf( stream, "\tsave-geometry yes\n" );
	else fprintf( stream, "\tsave-geometry no\n" );
	fprintf( stream, "\tMC-engine %s\n", globber_opts.fMC );
	if( globber_opts.fR3BMagnet ) fprintf( stream, "\tuse-magnet yes\n" );
	else fprintf( stream, "\tuse-magnet no\n" );
	fprintf( stream, "\toutput-file %s\n", globber_opts.OutFile );
	fprintf( stream, "\tpar-file %s\n", globber_opts.ParFile );
	fprintf( stream, "\tfield-scale %f\n", globber_opts.field_scale );
	if( strlen( globber_opts.material_file ) )
		fprintf( stream, "\tmat-file %s\n", globber_opts.material_file );
	fputs( "endsection\n", stream );
}

void r3b_ascii_dump_cuts( FILE *stream ){
	fputs( "#these are the cut specifications\n", stream );
	fputs( "section cuts\n", stream );
	
	float *cuts_av = (float*)&globber_cuts;
	const char *names[] = {
		"gamma",
		"electron",
		"neutral_had",
		"charged_had",
		"muon",
		"electron_bremss",
		"muon_bremss",
		"electron_delta",
		"muon_delta",
		"muon_pair",
		"max_ToF" };
	
	for( int i=0; i < 11; ++i )
		fprintf( stream, "\t%s %f\n", names[i], cuts_av[i] );
	fputs( "endsection\n", stream );
}

void r3b_ascii_dump_detectors( FILE *stream ){
	fputs( "#these are the active detectors\n", stream );
	fputs( "section detectors\n", stream );
	
	for( std::map<std::string, std::string>::iterator i=globber_opts.fDetlist.begin();
	     i != globber_opts.fDetlist.end(); i++ )
		fprintf( stream, "\t%s %s\n", i->first.c_str(), i->second.c_str() );
	fputs( "endsection\n", stream );
}

void r3b_ascii_dump_g3( FILE *stream ){
	char *g3_chav = &globber_g3.ckov;
	
	const char *names[] = {
		"ckov",
		"rayl",
		"pair",
		"comp",
		"phot",
		"pfis",
		"dray",
		"anni",
		"brem",
		"hadr",
		"munu",
		"dcay",
		"loss",
		"muls",
		"stra",
		"sync"
	};
	
	fputs( "#These are the Geant3 settings. You can find a better explanation in the docs.\n", stream );
	fputs( "section geant3\n", stream );
	
	fprintf( stream, "\ttrig %d\n", globber_g3.trig );
	fprintf( stream, "\tswit %d %d\n", globber_g3.swit[0], globber_g3.swit[1] );
	fprintf( stream, "\tdebu %d %d %d\n", globber_g3.debu[0], globber_g3.debu[1], globber_g3.debu[2] );
	fprintf( stream, "\tauto %d\n", globber_g3.gauto );
	fprintf( stream, "\taban %d\n", globber_g3.aban );
	fprintf( stream, "\topti %d\n", globber_g3.opti );
	fprintf( stream, "\teran %.10f\n", globber_g3.eran );
	
	for( int i=0; i < 16; ++i )
		fprintf( stream, "\t%s %d\n", names[i], (int)g3_chav[i] );
	fputs( "endsection\n", stream );
}
