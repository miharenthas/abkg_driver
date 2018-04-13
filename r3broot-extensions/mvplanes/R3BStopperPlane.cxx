/*******************************************************
//Author: L. Zanetti (lzanetti@ikp.tu-darmstadt.de)
//Date: 26.04.2017
//Description: Implementation of the class
//             R3BRattlePlane
*******************************************************/

#include "R3BStopperPlane.h"

//------------------------------------------------------------------------------------
//ctors:

//------------------------------------------------------------------------------------
//default:
R3BStopperPlane::R3BStopperPlane():
	R3BRattlePlane() //just call the RP's default.
{}

//------------------------------------------------------------------------------------
//parametric:
R3BStopperPlane::R3BStopperPlane( rp_specs &specs, const char *the_name, bool active ):
	R3BRattlePlane( specs, the_name, active ) //just call the RP's parametric.
{}

//------------------------------------------------------------------------------------
//How to process a hit. Which is the same as an entering track for the RP
//BUT it kills the track immediately.
Bool_t R3BStopperPlane::ProcessHits( FairVolume *fair_vol ){
	//if the track is entering, set the data
	//which is what we are actually interested in.
	R3BRPHit *current_hit;
	
	//if we have tracks that are created inside the detector
	//kill them and exit.
	if( gMC->IsNewTrack() ){
		gMC->StopTrack();
		return kTRUE;
	}

	if( !_is_new_event ){ //the previous event never closed
		_hits.back()->Finish(); //close it
	}
	//the event begins (not really useful, since they die here)
	_is_new_event = false;
	
	//set our handy pointer to a new hit.
	current_hit = new R3BRPHit;
	current_hit->_issuing_rattler = _own_index; //save the index of the plane
	
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
	current_hit->SetDetectorID( STOPPERPLANE_DETECTOR_ID );
	current_hit->SetEventID( gMC->CurrentEvent() );
	
	//retrieve (immediately) the hierarchy info
	//NOTE: this is a number which represent the internal Monte Carlo
	//      designation. If this number is -1, then it's a primary.
	current_hit->_parent_id = gMC->GetStack()->GetCurrentParentTrackNumber();
	
	current_hit->_tod = gMC->TrackTime(); //time of departure
	gMC->TrackPosition( current_hit->_pod ); //position of departure
	gMC->TrackMomentum( current_hit->_mad ); //momentum at departure
	current_hit->SetLength( gMC->TrackLength() ); //the length of the track...
	
	//set the FairMCPoint parent object
	current_hit->Finish();

	//do something to the stack
	((R3BStack*)gMC->GetStack())->AddPoint( RATTLEPLANE_DETECTOR_ID );
	
	//save 
	_hits.push_back( current_hit );
	
	//kill the track
	gMC->StopTrack();
	
	//the event is over
	_is_new_event = true;

	return kTRUE;
}

//interpreter garbage
ClassImp( R3BStopperPlane );
