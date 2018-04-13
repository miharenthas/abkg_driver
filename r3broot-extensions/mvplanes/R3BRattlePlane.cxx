/*******************************************************
//Author: L. Zanetti (lzanetti@ikp.tu-darmstadt.de)
//Special thanks: Paulina Mueller (pmueller@ikp.tu-darmstadt.de)
//Date: 15.02.2017
//Description: Implementation of the class
//             R3BRattlePlane
*******************************************************/

#include "R3BRattlePlane.h"

//------------------------------------------------------------------------------------
//init the index
int R3BRattlePlane::_rattler_index = 0;

//------------------------------------------------------------------------------------
//ctors:

//------------------------------------------------------------------------------------
//default:
R3BRattlePlane::R3BRattlePlane():
	R3BDetector( "Nebuchadnezzar", true, RATTLEPLANE_DETECTOR_ID ),
	_rattle_hits( new TClonesArray( "R3BRPHit", 1024 ) ),
	_is_new_event( true ),
	_own_index( R3BRattlePlane::_rattler_index )
{

	++R3BRattlePlane::_rattler_index;

}

//------------------------------------------------------------------------------------
//parametric:
R3BRattlePlane::R3BRattlePlane( rp_specs &specs, const char *the_name, bool active ):
	R3BDetector( the_name, active, RATTLEPLANE_DETECTOR_ID ),
	_rattle_hits( new TClonesArray( "R3BRPHit", 1024 ) ),
	_is_new_event( true ),
	_own_index( R3BRattlePlane::_rattler_index )
{
	//copy the given transformation
	_specs = specs;
	
	//get an index
	++R3BRattlePlane::_rattler_index;
}

//------------------------------------------------------------------------------------
//copy:
R3BRattlePlane::R3BRattlePlane( const R3BRattlePlane &given ):
	R3BDetector( given._name.c_str(), true, RATTLEPLANE_DETECTOR_ID ),
	_rattle_hits( new TClonesArray( *given._rattle_hits ) ),
	_is_new_event( given._is_new_event ),
	_own_index( R3BRattlePlane::_rattler_index )
{
	_specs = given._specs;
	
	//get an index: always incremental
	++R3BRattlePlane::_rattler_index;
}

//------------------------------------------------------------------------------------
//operators:

//------------------------------------------------------------------------------------
//assignment. Note that the TClonesArray doesn't just copy the pointers, but
//also duplicates its contents. So it's OK to do this and we don't get tangled.
R3BRattlePlane &R3BRattlePlane::operator=( R3BRattlePlane &right ){
	_rattle_hits = right._rattle_hits;
	_specs = right._specs;
	
	return *this;
}

//------------------------------------------------------------------------------------
//methods:

//------------------------------------------------------------------------------------
//Init the detector (basically, call some fair functions and put a line in the log).
void R3BRattlePlane::Initialize(){
	LOG( INFO ) << "R3BRattlePlane \"" << _name
	            << "\" is being itialized..." << FairLogger::endl;
	
	FairDetector::Initialize();
}

//------------------------------------------------------------------------------------
//ProcessHit method: does the thing necessary when the plane is stroke by a particle.
//To the best of my understanding, this gets called by the FairRunSim thingie.
Bool_t R3BRattlePlane::ProcessHits( FairVolume *the_volume ){
	//if the track is entering, set the data
	//which is what we are actually interested in.
	R3BRPHit *current_hit;
	if( gMC->IsTrackEntering() ){
		if( !_is_new_event ){ //the previous event never closed
			_hits.back()->Finish(); //close it
		}
		
		//set our handy pointer to a new hit.
		current_hit = new R3BRPHit;
		current_hit->_issuing_rattler = _own_index; //save the index of the plane
		_is_new_event = false; //toggle the event status
		
		current_hit->_e_loss = gMC->Edep(); //init the energy loss into the rattleplane
		current_hit->_toa = gMC->TrackTime(); //time of arrival
		gMC->TrackPosition( current_hit->_poe ); //point of entry
		gMC->TrackMomentum( current_hit->_mae ); //momentum at entry
		current_hit->_mass = gMC->TrackMass(); //get the mass
		current_hit->_charge = gMC->TrackCharge(); //get the charge
		current_hit->_pdg = gMC->GetStack()->
		                         GetCurrentTrack()->
		                         GetPdgCode(); //the PDG code
		
		current_hit->SetTrackID( gMC->GetStack()->GetCurrentTrackNumber() );
		current_hit->SetDetectorID( RATTLEPLANE_DETECTOR_ID );
		current_hit->SetEventID( gMC->CurrentEvent() );
		
		//retrieve (immediately) the hierarchy info
		//NOTE: this is a number which represent the internal Monte Carlo
		//      designation. If this number is -1, then it's a primary.
		current_hit->_parent_id = gMC->GetStack()->GetCurrentParentTrackNumber();
		
		//save 
		_hits.push_back( current_hit );

	//else if the track is leaving the rattle plane, or is dead in it
	} else if( gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared() ) {
		current_hit = _hits.back();
		
		current_hit->_tod = gMC->TrackTime(); //time of departure
		gMC->TrackPosition( current_hit->_pod ); //position of departure
		gMC->TrackMomentum( current_hit->_mad ); //momentum at departure
		current_hit->SetLength( gMC->TrackLength() ); //the length of the track...
		
		//set the FairMCPoint parent object
		current_hit->Finish();

		//do something to the stack
		((R3BStack*)gMC->GetStack())->AddPoint( RATTLEPLANE_DETECTOR_ID );
		_is_new_event = true; //toggle the event status
	} else {
		//if it's just in it, just increment the energy
		current_hit = _hits.back();
		current_hit->_e_loss += gMC->Edep();
	}

	return kTRUE;
}

//------------------------------------------------------------------------------------
//Register: make the FairRootManager aware that we exist
void R3BRattlePlane::Register() {
	//make an unique name for the collection
	char name_buf[64];
	sprintf( name_buf, "Rattles_%06x", _own_index ); //can't be random here, sorry
	                                                 //at least if you want to be able
	                                                 //to conveniently join trees. 
	FairRootManager::Instance()->Register( name_buf, GetName(), _rattle_hits, kTRUE );
}

//------------------------------------------------------------------------------------
//GetCollection: returns a pointer to a TClonesArray. 
TClonesArray *R3BRattlePlane::GetCollection( Int_t iColl ) const {
	//copy the hits
	for( int i=0; i < _hits.size(); ++i ) (*_rattle_hits)[i] = _hits[i];
	
	if( !iColl ){  return _rattle_hits; }
	else { return NULL; }
}

//------------------------------------------------------------------------------------
//Reset: possibly, make ready for the next event.
//void R3BRattlePlane::Reset(){
void R3BRattlePlane::EndOfEvent(){
	for( int i=0; i < _hits.size(); ++i ) delete _hits[i];
	_hits.clear();
	
	_rattle_hits->Clear(); //NOTE: this seems to actually clear out also the
	                       //      elements of the array. Which is safe, in this case
	                       //      because we sent out a copy of it.
	_is_new_event = true;
}

//------------------------------------------------------------------------------------
//ConstructGeometry: the big deal of this class.
//A plane is created, RATTLEPLANE_THICKNESS cm thick, and transformed according to
//what's in _specs.
//NOTE: So you may be wondering, why are we defining everything in here and then just
//      leaving everything as an unnamed place in memory?
//      First: ROOT destroys the concept of scope, effectively, so the various tranforms
//             are still known and anyway when the program exits everything is released.
//      Second: it's very unelegant but, in order to make the macro DisplayEvent work
//              we need things like this, otherwise we stumble on a double free, because
//              ROOT tries to deallocate pointers enclosed in the class.
void R3BRattlePlane::ConstructGeometry(){
	//make the transformation. I'm given to understand that the order is respected,
	//so this should work --note: rotations and translaions are relative to the origin.
	char name_buf[128];
	
	//Names for the things are generated so that they are unique
	//this way, conflicts should be avoided in case we want more
	//than one rattleplane.
	strcpy( name_buf, "Hugh_" ); //make the template name
	R3BRattlePlane::mk_unique_name( name_buf ); //make the temp name.
	TGeoRotation *global_Rot = new TGeoRotation( name_buf );
	global_Rot->RotateX( _specs.rot_x );
	global_Rot->RotateY( _specs.rot_y );
	global_Rot->RotateZ( _specs.rot_z );

	strcpy( name_buf, "Steven_" ); //make the template name
	R3BRattlePlane::mk_unique_name( name_buf ); //make the temp name.
	TGeoTranslation *global_Trans = new TGeoTranslation( name_buf, 
	                                                     _specs.T_x,
	                                                     _specs.T_y,
	                                                     _specs.T_z );
	global_Rot->RegisterYourself();
	global_Trans->RegisterYourself();
	
	strcpy( name_buf, "Gerald_" ); //make the template name
	R3BRattlePlane::mk_unique_name( name_buf ); //make the temp name.
	TGeoCombiTrans *reference_trf = new TGeoCombiTrans( name_buf );
	reference_trf->SetRotation( *global_Rot ); //first, hopefully, rotation
	reference_trf->SetTranslation( *global_Trans ); //then, hopefully, translation
	reference_trf->RegisterYourself();
	
	//create the volume
	strcpy( name_buf, "Woodrow_" ); //make the template name
	R3BRattlePlane::mk_unique_name( name_buf ); //make the temp name.
	TGeoBBox *rp = new TGeoBBox( name_buf,
	                              .5*_specs.width,
	                              .5*_specs.height,
	                              .5*_specs.depth );
	
	//make the volume , out of Lead.
	strcpy( name_buf, "WoodyVol_" ); //make the template name
	R3BRattlePlane::mk_unique_name( name_buf ); //make the temp name.
	
	//set the material, out of R3BTargetAssemblyMedia
	TGeoMedium *p_med;
	switch( _specs.stuff ){
		case AIR:
			p_med = R3BTAM::Air();
			break;
		case VACUUM:
			p_med = R3BTAM::Vacuum(); //no, really?
			break;
		case SILICON:
			p_med = R3BTAM::Silicon();
			break;
		case COPPER:
			p_med = R3BTAM::Copper();
			break;
		case ALUMINIUM:
			p_med = R3BTAM::Aluminium();
			break;
		case IRON:
			p_med = R3BTAM::Iron();
			break;
		case LEAD : default :
			p_med = R3BTAM::Lead();
			break;
	}          
	TGeoVolume *rp_volume = new TGeoVolume( name_buf, rp, p_med );
	
	//and add it as sensitive
	//NOTE: this method is local and inherited form FariModule
	//      and seems to be the missing link between TGeo stuff and
	//      FairVolume stuff.
	AddSensitiveVolume( rp_volume );
	

	//register it in the wolrd
	TGeoVolume *p_world = gGeoManager->GetTopVolume();
	p_world->SetVisLeaves( kTRUE );

	p_world->AddNode( rp_volume, 1, reference_trf );
}


//------------------------------------------------------------------------------------
//a simple tool to generate unique-ish names
//NOTE: "name_buf" should be big enough to hold the full unique name
//      The unique name is just the template with an 8 cipher hex
///     appended to it.
void R3BRattlePlane::mk_unique_name( char *name_buf ){
	char *buf = (char*)calloc( strlen(name_buf)+8, 1 ); //6 needed, 8 for safety...
	strcpy( buf, name_buf );
	strcat( buf, "%06x" );
	sprintf( name_buf, buf, rand()%0xffffff );
	free( buf );
}

//and seed the sequence (done manually, externally)
void R3BRattlePlane::seed_unique_namer(){
	srand( time(NULL) );
}
	
//interpreter garbage
ClassImp( R3BRattlePlane );
