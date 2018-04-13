/*******************************************************
//Author: L. Zanetti (lzanetti@ikp.tu-darmstadt.de)
//Date: 06.01.2017
//Description: This class defines the geometry of the
//             reaction's chamber content. This class
//             produces a sphere that fits inside the
//             CrystalBall; a different geometry
//             will be needed for CALIFA.
//NOTE: the geometries here may be specific to the
//      experiment s412. If this is the case, some
//      extensions and specializations of the class
//      are adviseable.
//NOTE: original implementation of these geometries
//      has been done by J. Isaak (j.isaak@gsi.de)
*******************************************************/

#ifndef R3BTARGETATMOSPHERE__H
#define R3BTARGETATMOSPHERE__H

//ROOT includes
#include "TGeoMedium.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoMatrix.h"
#include "TGeoSphere.h"
#include "TGeoCone.h"

//R3BRoot includes
#include "R3BModule.h"
#include "R3BTargetAssemblyMedia.h" //for the necessary media

//stl includes
#include <string>


class R3BTargetAtmosphere : public R3BModule {
	public:
		//an enum to define the kind of atmostphere
		//that will be put inside the target chamber
		//NOTE: this 
		typedef enum R3BTargetAtmosphereKind {
			NONE = '@', //don't do anything (same as not instantiating the class)
			AIR, //put air inside the reaction chamber, long character 'A'
			VACUUM //put vacuum inside the reaction chamber, long character 'B'
		} atm_kind;
	
		//cotrs, dtor
		R3BTargetAtmosphere( const char * name,
		                     const char *Title="R3BTargetAtmosphere",
		                     atm_kind ak = NONE );
		R3BTargetAtmosphere() {};
		virtual ~R3BTargetAtmosphere() {};
		
		//a method that will construct AND REGISTER
		//the geometry of the thing
		//NOTE: the classes R3BTarget{Shielding,Wheel} will try to register
		//      to the atmosphere before resorting to world, this has to be
		//      instantiate BEFORE them to get the hierarchy:
		//      R3BTargetAtmosphere{
		//          R3BTargetShielding
		//          R3BTargetWheel
		//      }
		virtual void ConstructGeometry();

		//some gobbins
		virtual Bool_t CheckIfSensitive( std::string name ){ return kFALSE; };

		ClassDef( R3BTargetAtmosphere, 1 )

	protected:
		std::string _ta_name; //the name of the object?
		atm_kind _ak; //kind of atmosphere
};

#endif
