/*******************************************************
//Author: L. Zanetti (lzanetti@ikp.tu-darmstadt.de)
//Date: 05.01.2017
//Description: This class defines the geometries of the
//             target mountings (target wheel and
//             montage platform and other minor gobbins)
//             used inside the Crystallball array.
//NOTE: the geometries here may be specific to the
//      experiment s412. If this is the case, some
//      extensions and specializations of the class
//      are adviseable.
//NOTE: original implementation of these geometries
//      has been done by J. Isaak (j.isaak@gsi.de)
*******************************************************/

#ifndef R3BTARGETWHEEL__H
#define R3BTARGETWHEEL__H

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


class R3BTargetWheel : public R3BModule {
	public:
		//cotrs, dtor
		R3BTargetWheel(const char * name, const char *Title="R3BTargetWheel");
		R3BTargetWheel() {};
		virtual ~R3BTargetWheel() {};
		
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
		//      is added (and no further atmospheres are created after!)
		virtual void ConstructGeometry();

		//some gobbins
		virtual Bool_t CheckIfSensitive( std::string name ){ return kFALSE; };
		
		ClassDef( R3BTargetWheel, 1 )

	protected:
		std::string _tw_name; //the name of the object?
};

#endif
