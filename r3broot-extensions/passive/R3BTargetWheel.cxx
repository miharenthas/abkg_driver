/*******************************************************
//Author: L. Zanetti (lzanetti@ikp.tu-darmstadt.de)
//Date: 05.01.2017
//Description: Implementation of the class
//             R3BTargetWheel
*******************************************************/

#include "R3BTargetWheel.h"

//------------------------------------------------------------------------------------
//ctors:

//------------------------------------------------------------------------------------
//parametric:
R3BTargetWheel::R3BTargetWheel( const char *name, const char *title ):
	R3BModule( name, title )
{
	_tw_name = name;
}

//------------------------------------------------------------------------------------
//methods:

//------------------------------------------------------------------------------------
//this method construct and registers the geometry.
void R3BTargetWheel::ConstructGeometry(){
	//some declarations
	double dx, dy, dz;
	double thx, thy, thz;
	double phx, phy, phz;
	double rmin, rmax;
	double phi1, phi2;
	
	TGeoRotation *zeroRot = new TGeoRotation; //zero rotation
	TGeoCombiTrans *tZero = new TGeoCombiTrans( "tZero", 0., 0., 0., zeroRot );
	tZero->RegisterYourself();
	
	//----------------------------------------
	//The montage platform:
	
	//it's made out of copper:
	TGeoMedium *pMedCu = R3BTAM::Copper();
	
	//Combi transformation:
	dx = 0.000000;
	dy = -14.090000;
	dz = 0.000000;
	// Rotation:
	thx = 90.000000;    phx = 0.000000;
	thy = 0.000000;    phy = 0.000000;
	thz = 90.000000;    phz = 270.000000;
	TGeoRotation *pMatrix15 = new TGeoRotation( "", thx, phx, thy, phy, thz, phz );
	TGeoCombiTrans *pMatrix14 = new TGeoCombiTrans( "", dx, dy, dz, pMatrix15 );


	rmin = 2.750000;
	rmax = 18.000000;
	dz   = 0.150000;
	phi1 = 0.000000;
	phi2 = 360.000000;
	TGeoShape *pMontagePlatform = new TGeoTubeSeg( "MontagePlatform", rmin,
	                                               rmax, dz, phi1, phi2 );
	TGeoVolume *pMontagePlatformLog = new TGeoVolume( "MontagePlatformLog",
	                                                   pMontagePlatform,
	                                                   pMedCu );
	
	//----------------------------------------
	//the montage ring:
	
	//it's made of aluminium
	TGeoMedium *pMedAl = R3BTAM::Aluminium();
	
	dx = 0.000000;
	dy = -17.000000;
	dz = 0.000000;
	// Rotation: 
	thx = 90.000000;    phx = 0.000000;
	thy = 0.000000;    phy = 0.000000;
	thz = 90.000000;    phz = 270.000000;
	TGeoRotation *pMatrix17 = new TGeoRotation( "", thx, phx, thy, phy, thz, phz );
	TGeoCombiTrans *pMatrix16 = new TGeoCombiTrans( "", dx, dy, dz, pMatrix17 );

	rmin = 12.000000;
	rmax = 15.000000;
	dz   = 0.250000;
	phi1 = 0.000000;
	phi2 = 360.000000;
	TGeoShape *pMontageRing = new TGeoTubeSeg( "MontageRing", rmin,
	                                           rmax, dz, phi1, phi2 );
	TGeoVolume *pMontageRingLog = new TGeoVolume( "MontageRingLog",
	                                               pMontageRing,
	                                               pMedAl );
	
	//----------------------------------------
	//the actual target wheel:
	
	//The Air box for the target frame
	dx = 1.550000;
	dy = 1.550000;
	dz = 0.2;      
	TGeoShape *TargetFrame = new TGeoBBox( "pTargetFrame", dx, dy, dz );
	
	//Target wheel
	rmin = 0.410000;
	rmax = 10.00000;
	dz   = 0.100000;
	phi1 = 0.000000;
	phi2 = 360.000000;
	TGeoShape *targetWheelShape = new TGeoTubeSeg( "pTargetWheelShape", rmin,
	                                               rmax, dz, phi1, phi2 );

	// Create non-overlapping volumes
	TGeoCombiTrans *twZero = new TGeoCombiTrans( "twZero", 0., 0., 0., zeroRot );
	twZero->RegisterYourself();

	//Transformations for the frame
	TGeoCombiTrans *tShift = new TGeoCombiTrans( "tShift", 6.2400, 3.900, 0.0, zeroRot );
	tShift->RegisterYourself();

	TGeoCompositeShape *tWheel = new TGeoCompositeShape( "TargWheel",
	                                                     "pTargetWheelShape - pTargetFrame:tShift" );
	//this is still made of aluminium: recycling the previously created material.
	TGeoVolume * pTargetWheelLog = new TGeoVolume( "TargetWheelLog", tWheel, pMedAl );

	//Transformation
	dx = -6.240000;
	dy = -3.900000;
	dz = 0.000000;

	thx = 90.000000;    phx = 0.000000;
	thy = 90.000000;    phy = 90.000000;
	thz = 0.000000;    phz = 0.000000;
	TGeoRotation *pMatrix29 = new TGeoRotation( "", thx, phx, thy, phy, thz, phz );
	TGeoCombiTrans *pMatrix28 = new TGeoCombiTrans( "", dx, dy, dz, pMatrix29 );
	
	//----------------------------------------
	//the motor to rotate the target wheel
	
	//it's made out of iron
	TGeoMedium *pMedFe = R3BTAM::Iron();
	
	dx = -6.240000;
	dy = -3.900000;
	dz = -4.370000;
	//Rotation: 
	thx = 90.000000;    phx = 0.000000;
	thy = 90.000000;    phy = 90.000000;
	thz = 0.000000;    phz = 0.000000;
	TGeoRotation *pMatrix01 = new TGeoRotation( "", thx, phx, thy, phy, thz, phz );
	TGeoCombiTrans *pMatrix02 = new TGeoCombiTrans( "", dx, dy, dz, pMatrix01 );

	rmin = 0.1500000;
	rmax = 2.100000;
	dz   = 2.7900000;
	phi1 = 0.000000;
	phi2 = 360.000000;
	TGeoShape *ptargetMotor = new TGeoTubeSeg( "targetMotor", rmin,
	                                           rmax, dz, phi1, phi2 );
	TGeoVolume *ptargetMotorLog = new TGeoVolume( "targetMotorLog",
	                                              ptargetMotor,
	                                              pMedFe );
	
	//----------------------------------------
	//the motor holder:
	
	dx = 3.000000;
	dy = 6.100000;
	dz = 0.500000;
	TGeoShape *pMotorHolder1 = new TGeoBBox( "MotorHolder1", dx, dy, dz );
	TGeoVolume *pMotorHolderLog1 = new TGeoVolume( "MotorHolderLog1", pMotorHolder1, pMedAl );

	dx = 3.480000;
	dy = 2.300000;
	dz = 0.500000;
	TGeoShape *pMotorHolder2 = new TGeoBBox( "MotorHolder2", dx, dy, dz );
	TGeoVolume *pMotorHolderLog2 = new TGeoVolume( "MotorHolderLog2", pMotorHolder2, pMedAl ); 

	dx = -13.64000;
	dy = -7.70000;
	dz = -1.05000;
	//Rotation: 
	thx = 90.000000;    phx = 0.000000;
	thy = 90.000000;    phy = 90.00000;
	thz = 0.000000;    phz = 0.000000;
	TGeoRotation *pMatrix03 = new TGeoRotation( "", thx, phx, thy, phy, thz, phz );
	TGeoCombiTrans *pMatrix04 = new TGeoCombiTrans( "", dx, dy, dz, pMatrix03 );

	dx = -7.14000;
	dy = -3.90000;
	dz = -1.05000;

	TGeoRotation *pMatrix05 = new TGeoRotation( "", thx, phx, thy, phy, thz, phz );
	TGeoCombiTrans *pMatrix06 = new TGeoCombiTrans( "", dx, dy, dz, pMatrix05 );
	
	//----------------------------------------
	//and now, registering:
	
	//first, assemble everything into its own object
	TGeoVolume *tw_assembly = new TGeoVolumeAssembly( "target_wheel_assembly" );
	
	//then, add all its (newly created) children:
	tw_assembly->AddNode( pMotorHolderLog2, 1, pMatrix05 );
	tw_assembly->AddNode( pMotorHolderLog1, 1, pMatrix04 );
	tw_assembly->AddNode( ptargetMotorLog, 1, pMatrix02 );
	tw_assembly->AddNode( pTargetWheelLog, 1, pMatrix28 );
	tw_assembly->AddNode( pMontageRingLog, 1, pMatrix16 );
	tw_assembly->AddNode( pMontagePlatformLog, 1, pMatrix14 );
	
	tw_assembly->SetVisLeaves( kTRUE );
	
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
	
	pChamber->AddNode( tw_assembly, 1, tZero );
}

ClassImp( R3BTargetWheel )
