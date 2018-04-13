/*******************************************************
//Author: L. Zanetti (lzanetti@ikp.tu-darmstadt.de)
//Date: 04.01.2017
//Description: Implementation of the class
//             R3BTargetAssemblyMedia
*******************************************************/

#include "R3BTargetAssemblyMedia.h"

//------------------------------------------------------------------------------------
//static method in charge of making air
TGeoMedium *R3BTAM::Air(){
	//holders for:
	//mass number, charge number, something
	//density, something else, something more.
	double a, z, w, density, radl, absl, par[20];
	int nel, numed; //two indexes
	
	// Mixture: Air
	TGeoMedium *air;
	if( gGeoManager->GetMedium("Air") ){
		air = gGeoManager->GetMedium("Air");
	}else{
		nel = 2;
		density = 0.001290;
		TGeoMixture *pMat2 = new TGeoMixture( "Air", nel, density );
		a = 14.006740;   z = 7.000000;   w = 0.700000; // N
		pMat2->DefineElement( 0, a, z, w );
		a = 15.999400;   z = 8.000000;   w = 0.300000; // O
		pMat2->DefineElement( 1, a, z, w );
		pMat2->SetIndex(1);
		
		// Medium: Air
		numed = 1;  // medium number
		memset( par, 0, 20*sizeof(double) );
		air = new TGeoMedium( "Air", numed, pMat2, par );
	}
	
	//and return it
	return air;
}

//------------------------------------------------------------------------------------
//static method in charge of the skull's content...
TGeoMedium *R3BTAM::Vacuum(){
	//holders for:
	//mass number, charge number, something
	//density, something else, something more.
	double a, z, w, density, radl, absl, par[20];
	int nel, numed; //two indexes

	// Mixture: Vacuum
	TGeoMedium *vacuum;
	if (gGeoManager->GetMedium("Vacuum") ){
		vacuum = gGeoManager->GetMedium("Vacuum");
	}else{
		nel = 2;
		density = 0.0000001; //????????????????
		TGeoMixture* pMat33 = new TGeoMixture("Vacuum", nel,density);
		a = 14.006740; z = 7.000000; w = 0.700000;  // N
		pMat33->DefineElement(0,a,z,w);
		a = 15.999400; z = 8.000000; w = 0.300000;  // O
		pMat33->DefineElement(1,a,z,w);
		pMat33->SetIndex(32);
		
		//medium: Vacuuuuuuuuuum
		numed   = 1;  // medium number
		memset( par, 0, 20*sizeof(double) );
		vacuum = new TGeoMedium( "Vacuum", numed, pMat33, par );
	}
	
	//and return it
	return vacuum;
}

//------------------------------------------------------------------------------------
//static method in charge of creating sililcon
TGeoMedium *R3BTAM::Silicon(){
	//holders for:
	//mass number, charge number, something
	//density, something else, something more.
	double a, z, w, density, radl, absl, par[20];
	int nel, numed; //two indexes
	
	// Material: Silicon
	TGeoMedium * pMedSi;
	if (gGeoManager->GetMedium("Silicon") ){
		pMedSi=gGeoManager->GetMedium("Silicon");
	}else{
		a       = 28.090000;
		z       = 14.000000;
		density = 2.330000;
		radl    = 9.351106;
		absl    = 456.628489;
		TGeoMaterial*
			pMat22 = new TGeoMaterial("Silicon", a,z,density,radl,absl);

		pMat22->SetIndex(21);
		// Medium: Silicon
		numed   = 21;  // medium number

		par[0]  = 0.000000; // isvol
		par[1]  = 0.000000; // ifield
		par[2]  = 0.000000; // fieldm
		par[3]  = 0.000000; // tmaxfd
		par[4]  = 0.000000; // stemax
		par[5]  = 0.000000; // deemax
		par[6]  = 0.000100; // epsil
		par[7]  = 0.000000; // stmin

		pMedSi = new TGeoMedium( "Silicon", numed, pMat22, par );
	}
	
	//and return it
	return pMedSi;
}

//------------------------------------------------------------------------------------
//static method in charge of creating copper
TGeoMedium *R3BTAM::Copper(){
	//holders for:
	//mass number, charge number, something
	//density, something else, something more.
	double a, z, w, density, radl, absl, par[20];
	int nel, numed; //two indexes
	
	// Material: Copper
	TGeoMedium *pMedCu=NULL;
	if (gGeoManager->GetMedium("Copper") ){
		pMedCu=gGeoManager->GetMedium("Copper");
	}else{
		a       = 63.540000;
		z       = 29.000000;
		density = 8.960000;
		radl    = 1.435029;
		absl    = 155.874854;
		TGeoMaterial *pMat25 = new TGeoMaterial("Copper", a,z,density,radl,absl);
		pMat25->SetIndex(24);
		// Medium: Copper
		numed   = 24;  // medium number

		par[0]  = 0.000000; // isvol
		par[1]  = 0.000000; // ifield
		par[2]  = 0.000000; // fieldm
		par[3]  = 0.000000; // tmaxfd
		par[4]  = 0.000000; // stemax
		par[5]  = 0.000000; // deemax
		par[6]  = 0.000100; // epsil
		par[7]  = 0.000000; // stmin

		pMedCu = new TGeoMedium( "Copper", numed, pMat25, par );
	}
	
	//and return it
	return pMedCu;
}

//------------------------------------------------------------------------------------
//static method in charge of creating alluminium
TGeoMedium *R3BTAM::Aluminium(){
	//holders for:
	//mass number, charge number, something
	//density, something else, something more.
	double a, z, w, density, radl, absl, par[20];
	int nel, numed; //two indexes

	// Material: Aluminum
	TGeoMedium * pMedAl=NULL;
	if (gGeoManager->GetMedium("Aluminum") ){
		pMedAl=gGeoManager->GetMedium("Aluminum");
	}else{
		a       = 26.980000;
		z       = 13.000000;
		density = 2.700000;
		radl    = 8.875105;
		absl    = 388.793113;
		TGeoMaterial *pMat21 = new TGeoMaterial("Aluminum", a,z,density,radl,absl);
		pMat21->SetIndex(20);
		// Medium: Aluminum
		numed   = 20;  // medium number

		par[0]  = 0.000000; // isvol
		par[1]  = 0.000000; // ifield
		par[2]  = 0.000000; // fieldm
		par[3]  = 0.000000; // tmaxfd
		par[4]  = 0.000000; // stemax
		par[5]  = 0.000000; // deemax
		par[6]  = 0.000100; // epsil
		par[7]  = 0.000000; // stmin

		pMedAl = new TGeoMedium("Aluminum", numed,pMat21, par);
	}
	
	//and return it
	return pMedAl;
}

//------------------------------------------------------------------------------------
//static method in charge of creating iron
TGeoMedium *R3BTAM::Iron(){
	//holders for:
	//mass number, charge number, something
	//density, something else, something more.
	double a, z, w, density, radl, absl, par[20];
	int nel, numed; //two indexes

	TGeoMedium * pMedFe=NULL;
	if (gGeoManager->GetMedium("Iron") ){
		pMedFe=gGeoManager->GetMedium("Iron");
	}else{
		a       = 55.850000;
		z       = 26.000000;
		density = 7.870000;
		radl    = 1.757717;
		absl    = 169.994418;
		TGeoMaterial *pMatFe = new TGeoMaterial("Iron", a,z,density,radl,absl);
		pMatFe->SetIndex(701);
		numed   = 23;  // medium number
		par[0]  = 0.000000; // isvol
		par[1]  = 0.000000; // ifield
		par[2]  = 0.000000; // fieldm
		par[3]  = 0.000000; // tmaxfd
		par[4]  = 0.000000; // stemax
		par[5]  = 0.000000; // deemax
		par[6]  = 0.000100; // epsil
		par[7]  = 0.000000; // stmin
		pMedFe = new TGeoMedium("Iron", numed,pMatFe, par);
	}
	
	//and return it
	return pMedFe;
}

//------------------------------------------------------------------------------------
//static method in charge of creating lead
TGeoMedium *R3BTAM::Lead(){
	//holders for:
	//mass number, charge number, something
	//density, something else, something more.
	double a, z, w, density, radl, absl, par[20];
	int nel, numed; //two indexes
	
	TGeoMedium * pMedPb=NULL;
	if (gGeoManager->GetMedium("Lead") ){
		pMedPb=gGeoManager->GetMedium("Lead");
	}else{
		a       = 207.20000;
		z       = 82.000000;
		density = 13.340000;
		TGeoMaterial *pMatPb = new TGeoMaterial("Lead", a,z,density); //,radl,absl);
		pMatPb->SetIndex(702);
		numed   = 28;  // medium number
		par[0]  = 0.000000; // isvol
		par[1]  = 0.000000; // ifield
		par[2]  = 0.000000; // fieldm
		par[3]  = 0.000000; // tmaxfd
		par[4]  = 0.000000; // stemax
		par[5]  = 0.000000; // deemax
		par[6]  = 0.000100; // epsil
		par[7]  = 0.000000; // stmin
		pMedPb = new TGeoMedium("Lead", numed, pMatPb, par);
	}
	
	//and return it
	return pMedPb;
}

ClassImp( R3BTargetAssemblyMedia )
