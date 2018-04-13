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
R3BTargetShielding::R3BTargetShielding( const char *name, const char *title ):
	R3BModule( name, title )
{
	_ts_name = name;
}

//------------------------------------------------------------------------------------
//methods:

//------------------------------------------------------------------------------------
//this method construct and registers the geometry.
void R3BTargetShielding::ConstructGeometry(){
	//some declarations
	double dx, dy, dz;
	double thx, thy, thz;
	double phx, phy, phz;
	double rmin, rmax;
	double phi1, phi2;

	TGeoRotation *zeroRot = new TGeoRotation; //zero rotation
	TGeoCombiTrans *tZero = new TGeoCombiTrans( "tZero", 0., 0., 0., zeroRot );
	tZero->RegisterYourself();

	//get the materials
	TGeoMedium *pMedCu, *pMedPb;
	pMedCu = R3BTAM::Copper();
	pMedPb = R3BTAM::Lead();

	//------------------------------------	
	// make shape components
	rmin = 0.1;
	rmax = 18.0;
	dz   = 0.05;
	phi1 = 0.0;
	phi2 = 360.0;

	TGeoShape *outerCapRaw = new TGeoTubeSeg("outerCapRaw",rmin,rmax,dz,phi1,phi2);
	TGeoVolume *outerCapRawLog = new TGeoVolume("outerCapRawLog", outerCapRaw, pMedCu);

	rmin = 0.1;
	rmax = 17.85;
	dz   = 0.1;
	phi1 = 0.0;
	phi2 = 360.0;

	TGeoShape *innerCapRaw = new TGeoTubeSeg("innerCapRaw",rmin,rmax,dz,phi1,phi2);
	TGeoVolume *innerCapRawLog = new TGeoVolume("innerCapRawLog", innerCapRaw, pMedPb);

	rmin = 0.1;
	rmax = 18.0;
	dz   = 0.05;
	phi1 = 0.0;
	phi2 = 360.0;

	TGeoShape *outerBottomRaw = new TGeoTubeSeg("outerBottomRaw",rmin,rmax,dz,phi1,phi2);
	TGeoVolume *outerBottomRawLog = new TGeoVolume("outerBottomRawLog", outerBottomRaw, pMedCu);

	rmin = 0.1;
	rmax = 17.9;
	dz   = 0.1;
	phi1 = 0.0;
	phi2 = 360.0;

	TGeoShape *innerBottomRaw = new TGeoTubeSeg("innerBottomRaw",rmin,rmax,dz,phi1,phi2);
	TGeoVolume *innerBottomRawLog = new TGeoVolume("innerBottomRawLog", innerBottomRaw, pMedPb);

	rmin = 17.9;
	rmax = 18.0;
	dz   = 10.5;
	phi1 = 0.0;
	phi2 = 360.0;

	TGeoShape *outerSurfaceRaw = new TGeoTubeSeg("outerSurfaceRaw",rmin,rmax,dz,phi1,phi2);
	TGeoVolume *outerSurfaceRawLog = new TGeoVolume("outerSurfaceRawLog", outerSurfaceRaw, pMedCu);


	rmin = 17.7;
	rmax = 17.9;
	dz   = 10.3;
	phi1 = 0.0;
	phi2 = 360.0;

	TGeoShape *innerSurfaceRaw = new TGeoTubeSeg("innerSurfaceRaw",rmin,rmax,dz,phi1,phi2);
	TGeoVolume *innerSurfaceRawLog = new TGeoVolume("innerSurfaceRawLog", innerSurfaceRaw, pMedPb);

	//part so subtract from bottom parts and surface (see technical drawing)
	TGeoShape *part1 = new TGeoBBox( "part1", 0.7, 3.95, 0.1 );
	TGeoShape *part2 = new TGeoBBox( "part2", 0.3, 3.05, 0.1 );

	TGeoShape *hole = new TGeoTubeSeg( "hole", 0.0, 2.73, 30.0, 0.0, 360.0 );
	TGeoVolume *holeLog = new TGeoVolume( "holeLog", hole, pMedPb );

	//------------------------------------
	//define some rotations and translations
	TGeoRotation *r1 = new TGeoRotation("r1",90,90,0);
	r1->RegisterYourself();
	TGeoCombiTrans *c1 = new TGeoCombiTrans("c1", 0.0,0.0,3.5,r1);
	c1->RegisterYourself();



	TGeoTranslation *t1 = new TGeoTranslation("t1", -1.6, -13.95, 0.0); 
	t1->RegisterYourself();
	TGeoTranslation *t2 = new TGeoTranslation("t2", 0.0, -6.25, 0.0); 
	t2->RegisterYourself();

	TGeoTranslation *trans1 = new TGeoTranslation("trans1", 0.0, 0.0, 0.15); 
	trans1->RegisterYourself();

	TGeoTranslation *trans2 = new TGeoTranslation("trans2", 0.0, 0.0, 10.55); 
	trans2->RegisterYourself();

	TGeoTranslation *trans3 = new TGeoTranslation("trans3", 0.0, 0.0, 10.55); 
	trans3->RegisterYourself();

	TGeoTranslation *trans4 = new TGeoTranslation("trans4", 0.0, 0.0, 20.95); 
	trans4->RegisterYourself();

	TGeoTranslation *trans5 = new TGeoTranslation("trans5", 0.0, 0.0, 21.1); 
	trans5->RegisterYourself();
	
	//final/last rotation matrix:
	//Combi transformation: 
	dx = 0.0000;
	dy = -14.0000;
	dz = 0.0000;
	//Rotation: 
	thx = 90.000000;    phx = 0.000000;
	thy = 90.000000;    phy = 90.000000;
	thz = 0.000000;    phz = 0.000000;	
	
	TGeoRotation *gRot = new TGeoRotation();
	gRot->RotateX(-90.0);
	gRot->RotateY(-90.0);
	gRot->RotateZ(0.0);
	
	TGeoRotation *pMatrix100 = new TGeoRotation("",thx,phx,thy,phy,thz,phz);
	TGeoCombiTrans *pMatrix101 = new TGeoCombiTrans("", dx,dy,dz,gRot);
	
	//------------------------------------
	//create bottom, cap and surface
	
	//bottom
	TGeoCompositeShape *innerBottom = new TGeoCompositeShape( "innerBottom",
	                                                          "(innerBottomRaw - part1:t1) - part2:t2" );
	TGeoVolume *innerBottomLog = new TGeoVolume("innerBottomLog", innerBottom, pMedPb);

	TGeoCompositeShape *outerBottom = new TGeoCompositeShape( "outerBottom",
	                                                          "(outerBottomRaw-part1:t1)-part2:t2" );
	TGeoVolume *outerBottomLog = new TGeoVolume("outerBottomLog", outerBottom, pMedCu);
	
	//surface
	TGeoCompositeShape *outerSurface = new TGeoCompositeShape( "outerSurface",
	                                                           "outerSurfaceRaw-hole:c1" );
	TGeoVolume *outerSurfaceLog = new TGeoVolume("outerSurfaceLog",outerSurface, pMedCu);

	TGeoCompositeShape *innerSurface = new TGeoCompositeShape( "innerSurface", 
	                                                           "innerSurfaceRaw-hole:c1" );
	TGeoVolume *innerSurfaceLog = new TGeoVolume( "innerSurfaceLog", innerSurface, pMedPb );

	//------------------------------------
	//combine all parts together to have the full lead and copper shielding
	TGeoCompositeShape *leadShielding =
		new TGeoCompositeShape( "leadShielding",
	                          "innerBottom:trans1+innerSurface:trans2+innerCapRaw:trans4");
	TGeoVolume *leadShieldingLog = new TGeoVolume( "leadShieldingLog", leadShielding, pMedPb );

	TGeoCompositeShape *copperShielding =
		new TGeoCompositeShape( "copperShielding",
		                        "outerBottom+outerSurface:trans3+outerCapRaw:trans5" );
	TGeoVolume *copperShieldingLog = new TGeoVolume( "copperShieldingLog", copperShielding, pMedCu );

	//------------------------------------
	//do the registering
	
	//first, assemble everything into one single entity
	TGeoVolume *target_shielding = new TGeoVolumeAssembly( "target_schielding" );
	
	//then, add the children
	target_shielding->AddNode( leadShieldingLog, 1, pMatrix101 );
	target_shielding->AddNode( copperShieldingLog, 1, pMatrix101 );
	
	//lastly, register the assembly
	//get the top shape of the wolrd (the cave or the target atmosphere, if defined)
	TGeoVolume *pChamber = gGeoManager->GetVolume( "reaction_chamber_log" );
	if( !pChamber ){
		R3BTargetAtmosphere atm( "target_atmosphere",
		                         "R3BTargetAtmosphere",
		                         R3BTargetAtmosphere::VACUUM );
		atm.ConstructGeometry();
		pChamber = gGeoManager->GetVolume( "reaction_chamber_log" );

	}
	pChamber->SetVisLeaves( kTRUE );
	
	pChamber->AddNode( target_shielding, 1, tZero );

}

ClassImp( R3BTargetShielding )
