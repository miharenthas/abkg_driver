#include "R3BMagnet.h"

//positional information

#define __ALADIN_DX 0
#define __ALADIN_DY 0
#define __ALADIN_DZ 255
#define __ALADIN_RX 0
#define __ALADIN_RY -7
#define __ALADIN_RZ 0


using namespace std;


R3BMagnet::~R3BMagnet()
{
}


R3BMagnet::R3BMagnet()
{
}


R3BMagnet::R3BMagnet(const char * name, const char *Title)
  : R3BModule(name ,Title)
{
}


R3BMagnet::R3BMagnet( const TString &geofile ):
	R3BModule( "Aladin", "TheMagnet", kFALSE, geofile,
                   { __ALADIN_DX, __ALADIN_DY, __ALADIN_DZ } ){
        TGeoRotation rot;
        rot.RotateY( __ALADIN_RY );
	R3BModule::SetRotation( rot );
}	

void R3BMagnet::ConstructGeometry()
{
  TString fileName = GetGeometryFileName();
  if(fileName.EndsWith(".root")) {
    LOG(INFO) << "Constructing ALADIN geometry from ROOT file " << fileName.Data() << FairLogger::endl;
    ConstructRootGeometry();
  } else {
    LOG(FATAL) << "ALADIN Geometry file is not specified" << FairLogger::endl;
    exit(1);
  }
}


Bool_t R3BMagnet::CheckIfSensitive(std::string name)
{
	return kFALSE;
}


ClassImp(R3BMagnet)
