/*******************************************************
//Author: L. Zanetti (lzanetti@ikp.tu-darmstadt.de)
//Date: 05.01.2017
//Description: Implementation of the class
//             R3BTargetShielding
*******************************************************/

#include "R3BTargetShielding.h"

//------------------------------------------------------------------------------------
//ctors:

//------------------------------------------------------------------------------------
//parametric:
R3BTargetAtmosphere::R3BTargetAtmosphere( const char *name,
                                          const char *title,
                                          R3BTargetAtmosphere::atm_kind ak ):
	R3BModule( name, title ),
	_ak( ak ),
	_ta_name( name )
{}

//------------------------------------------------------------------------------------
//methods:

//------------------------------------------------------------------------------------
//this method construct and registers the geometry.
void R3BTargetAtmosphere::ConstructGeometry(){
	//some declarations
	double dx, dy, dz;
	double thx, thy, thz;
	double phx, phy, phz;
	double rmin, rmax, rmin1, rmax1, rmin2, rmax2;
	double phi1, phi2;

	//check if an atmosphere already exists
	//if so, do nothing.
	if( gGeoManager->GetVolume( "reaction_chamber_log" ) ||
	    gGeoManager->GetVolume( "aTraMedium_ReactionChamberLog" ) ) return;

	//set the filling medium according to how the class has been
	//constructed. Return without doing anything if no material
	//has been set
	TGeoMedium *pMedFill;
	switch( _ak ){
		case R3BTargetAtmosphere::AIR :
			pMedFill = R3BTAM::Air();
			break;
		case R3BTargetAtmosphere::VACUUM :
			pMedFill = R3BTAM::Vacuum();
			break;
		case R3BTargetAtmosphere::NONE : default :
			return;
	}

	TGeoRotation *zeroRot = new TGeoRotation; //zero rotation
	TGeoCombiTrans *tZero = new TGeoCombiTrans( "tZero", 0., 0., 0., zeroRot );
	tZero->RegisterYourself();

	//inner reaction chamber volume
	//No transformation required for this one.
	TGeoShape *reaction_ch = new TGeoSphere( "reaction_chamber", 0., 24.250000 ); 
	TGeoVolume *reaction_chLog = new TGeoVolume( "reaction_chamber_log",
	                                             reaction_ch, pMedFill );
	
	//three conical in out and bottom beam tubes.
	//need additional short cylinders in order to match chamber with cones.
	//first: create the concical tube itself (will be copied)
	dz = 14.600000;
	rmin1 = 0.0;
	rmax1 = 2.730000;
	rmin2 = 0.0;
	rmax2 = 5.125000;
	phi1 = 0.000000;
	phi2 = 360.000000;
	TGeoShape *conical_tube = new TGeoConeSeg( "conical_tube",
                                                   dz, rmin1, rmax1, rmin2,
                                                   rmax2, phi1, phi2 );
	TGeoVolume *conical_tubeLog = new TGeoVolume( "conical_tubeLog",
                                                      conical_tube, pMedFill );
	//transformations:
	dx = 0.000000;
	dy = 0.000000;
	dz = 39.100000;
	// Rotation: 
	TGeoCombiTrans* pCombTrans1 = new TGeoCombiTrans("", dx,dy,dz, zeroRot);
	// Combi transformation: 
	dx = 0.000000;
	dy = 0.000000;
	dz = -39.100000;
	// Rotation:
	TGeoRotation *pRot1 = new TGeoRotation( "", 0, 180, 0 );
	TGeoCombiTrans* pCombTrans2 = new TGeoCombiTrans( "", dx,dy,dz, pRot1 );
	// Combi transformation:
	dx = 0.000000; 
	dy = -39.100000;
	dz = 0.000000;
	// Rotation: 
	TGeoRotation *pRot2 = new TGeoRotation("", 0, 90, 0);
	TGeoCombiTrans* pCombTrans3 = new TGeoCombiTrans("", dx,dy,dz, pRot2);
	
	//add the tubes to the chamber's volume.
	reaction_chLog->AddNode(conical_tubeLog, 0, pCombTrans1);
	reaction_chLog->AddNode(conical_tubeLog, 1, pCombTrans2);
	reaction_chLog->AddNode(conical_tubeLog, 2, pCombTrans3);
	
	//need in and out bridging filling cylindrical volumes; also: bottom hole volume
	//placement
	dx = 0.000000;
	dy = 0.000000;
	dz = 24.500000;	//cm, at reaction chamber edge
	TGeoCombiTrans* pCombTrans4 = new TGeoCombiTrans("", dx,dy,dz, zeroRot);
	TGeoCombiTrans* pCombTrans5 = new TGeoCombiTrans("", dx,dy,-dz, zeroRot);
	dx = 0.000000; 
	dy = -24.500000;
	dz = 0.000000;
	TGeoCombiTrans* pCombTrans6 = new TGeoCombiTrans("", dx,dy,dz, pRot2);
	//shape
	rmin = 0.0;
	rmax = 2.730000;
	dz = 1.000000;	//2cm thickness (length)
	phi1 = 0.0;
	phi2 = 360.0;
	TGeoShape *ch_bridge_Tube = new TGeoTubeSeg( "ch_bridge_Tube",
	                                             rmin, rmax, dz, phi1, phi2 );
	TGeoVolume *ch_bridge_TubeLog = new TGeoVolume( "ch_bridge_TubeLog",
	                                                ch_bridge_Tube, pMedFill );
	reaction_chLog->AddNode(ch_bridge_TubeLog, 0, pCombTrans4);
	reaction_chLog->AddNode(ch_bridge_TubeLog, 1, pCombTrans5);
	reaction_chLog->AddNode(ch_bridge_TubeLog, 2, pCombTrans6);

	//------------------------------------
	//do the registering
	
	//get the top shape of the wolrd (basically, the cave)
	TGeoVolume *pWorld = gGeoManager->GetTopVolume();
	pWorld->SetVisLeaves( kTRUE );
	
	pWorld->AddNode( reaction_chLog, 1, tZero );
}

ClassImp( R3BTargetAtmosphere )
