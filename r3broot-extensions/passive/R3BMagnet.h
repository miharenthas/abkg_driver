#ifndef R3BMAGNET_H
#define R3BMAGNET_H

#include <iostream>
#include <stdlib.h>

#include "TNamed.h"
#include "TArrayI.h"
#include "TClonesArray.h"
#include "FairDetector.h"
#include "FairModule.h"
#include "R3BModule.h"
#include "TGeoMatrix.h"
#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
#include "FairGeoRootBuilder.h"
#include "FairRuntimeDb.h"
#include "TObjArray.h"
#include "FairRun.h"
#include "FairGeoVolume.h"
#include "FairGeoNode.h"

#include "R3BGeoMagnet.h"
#include "R3BGeoPassivePar.h"

class R3BMagnet : public R3BModule {
  
public:
  R3BMagnet(const char * name, const char *Title );
  R3BMagnet( const TString &geofile );
  R3BMagnet();
  virtual ~R3BMagnet();
  
  void SetPosition( const TGeoTranslation & ) {};
  void SetRotation( const TGeoRotation & ) {};

  void ConstructGeometry();
  
  Bool_t CheckIfSensitive(std::string name);
  
  ClassDef(R3BMagnet,2) //R3BMagnet
};

#endif //R3BMAGNET_H

