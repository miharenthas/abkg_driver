/*******************************************************
//Author: L. Zanetti (lzanetti@ikp.tu-darmstadt.de)
//Date: 05.01.2017
//Description: This class defines the geometries of the
//             target shielding in two different layers:
//             the outer copper covering and the inner
//             lead shielding.
//NOTE: the geometries here may be specific to the
//      experiment s412. If this is the case, some
//      extensions and specializations of the class
//      are adviseable.
//NOTE: original implementation of these geometries
//      has been done by J. Isaak (j.isaak@gsi.de)
*******************************************************/

#ifndef R3BTARGETSHIELDING__H
#define R3BTARGETSHIELDING__H

//ROOT includes
#include "TGeoMedium.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoShape.h"
#include "TGeoMatrix.h"
#include "TGeoTube.h"
#include "TGeoCompositeShape.h"
#include "TGeoBBox.h"

//R3BRoot includes
#include "R3BModule.h"
#include "R3BTargetAssemblyMedia.h" //for the necessary media
#include "R3BTargetAtmosphere.h" //for MakeGeometry()

//stl includes
#include <string>


class R3BTargetShielding : public R3BModule {
	public:
		//cotrs, dtor
		R3BTargetShielding(const char * name, const char *Title="R3BTargetShielding");
		R3BTargetShielding() {};
		virtual ~R3BTargetShielding() {};
		
		//a method that will construct AND REGISTER
		//the geometry of the thing
		//NOTE: since this class will try to register first to "reaction_chamber"
		//      which contains the information on the reaction chamber filling
		//      it is adviesable to instantiate this AFTER the atmosphere, in order
		//      to construct the correct hierarchy:
		//      R3BTargetAtmosphere{
		//          R3BTargetShielding
		//          R3BTargetWheel
		//      }
		//      if the atmosphere isn't instantiated yet, a vacuum filling
		//      is added (and no further atmospheres are created!)
		virtual void ConstructGeometry();

		//some gobbins
		virtual Bool_t CheckIfSensitive( std::string name ){ return kFALSE; };

		ClassDef( R3BTargetShielding, 1 )

	protected:
		std::string _ts_name; //the name of the object?
};

#endif
