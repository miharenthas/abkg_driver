/*******************************************************
//Author: L. Zanetti (lzanetti@ikp.tu-darmstadt.de)
//Special thanks: Paulina Mueller (pmueller@ikp.tu-darmstadt.de)
//Date: 15.02.2017
//Description: This class defines a giant plane detector
//             that will register energy, momentum and
//             coordinates of any incoming particle.
//Usage: Unlike other detectors, this one can (and should)
//       be placed at runtime. To do so, you have to pass an
//       instace of the rp_specs structure, carrying information
//       about the position and rotation of the plane.
//       By default, the plane will be created at the origin
//       and perpendicular to the beam (Z) axis.
//NOTE: needless to say, this is a probe for the simulation
//      not a real detector.
*******************************************************/

#ifndef R3BRATTLEPLANE__H
#define R3BRATTLEPLANE__H

//define a constant for the detector ID
#define RATTLEPLANE_DETECTOR_ID kRATTLEPLANE

#include <string>

//unique name generation
#include <stdlib.h>
#include <string.h>

#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include "TVirtualMC.h" //for gMC
#include "TGeoManager.h" //for gGeoManager

#include "FairLogger.h"
#include "FairRootManager.h"

#include "R3BMCStack.h" //for R3BStack
#include "R3BDetectorList.h" //for kRATTLEPLANE
#include "R3BDetector.h" //parent class
#include "R3BRPHit.h" //the rattle of the rattle plane
#include "R3BTargetAssemblyMedia.h" //This is here to provide materials, even
                                    //if this is not strictly its intended
                                    //application. Still, should work.

class R3BRattlePlane : public R3BDetector {
	public:
		//a handy structure to pass the information of the transformation
		typedef struct _r3brattleplane_specifications {
			double rot_x, rot_y, rot_z;
			double T_x, T_y, T_z;
			double width, height, depth;
			R3BTAM_switcher stuff; //the material, wide character friednly
		} rp_specs;

		//ctors, dtor.
		//As mentioned above, if not default constructed, this class
		//should be given an affine transformation to place the plane.
		R3BRattlePlane();
		//parametric constructor:
		// -- specs: the spect structure containig the measures and
		//           placement of the rattleplane.
		//           NOTE: there seem to be a slight problem with the
		//                 rotations (02.03.2017)
		// -- name: a string to name the thing. "Nebuchadnezzar" is the default.
		// -- active: a flag to switch on and off the detector.
		R3BRattlePlane( rp_specs &specs, const char *the_name, bool active );
		
		virtual ~R3BRattlePlane(){};
		
		//implementation of the various abstract method of the parent class
		virtual void Initialize();
		virtual Bool_t ProcessHits( FairVolume *fair_vol = NULL );
		virtual void EndOfEvent();
		virtual void Reset() { /*do nothing*/ };
		virtual void Register();
		virtual TClonesArray *GetCollection( Int_t iColl ) const; //retrieve the data
		                                                          // -- iColl: is the
		                                                          //    "index" of the
		                                                          //    collection (data
		                                                          //    structure)
		virtual void Print( Option_t *opt = "" ){ /*oh please*/ };
		virtual void ConstructGeometry(); //A very important method this one: the geometry
		                                  //of the detector is made in here and it's the
		                                  //whole point of this tatty class.
		//virtual void PostTrack(); //???
		virtual Bool_t CheckIfSensitive( std::string ){ return kTRUE; }; //it is sensitive

		//make a unique (?) name from a template
		static void mk_unique_name( char *name_template );
		static void seed_unique_namer();

		//interpreter garbage
		ClassDef( R3BRattlePlane, 1 );
	protected: //i'm actually expecting to derive single neuLAND planes from this,
	           //because of bone-eyed lazyness. Stay tuned.
		TClonesArray *_rattle_hits; //it's a container for the event passing
		std::vector<R3BRPHit*> _hits; //the SANE container for the hits.
		rp_specs _specs; //the specs
		std::string _name; //keep track of the name

		bool _is_new_event; //new event toggle (useful)
		
		//indexing (a bit bone headed, admittedly)
		//more to the point, this is not thread safe.
		//But it doesn't really matter here, since ROOT
		//is not thread safe.
		static int _rattler_index;
		const int _own_index;
	private:
		R3BRattlePlane( const R3BRattlePlane &given ); //since we are burly programmers,
		                                               //this will support copy construction.
		//and, sure enough, we also do an assignment operator, shall we?
		R3BRattlePlane &operator=( R3BRattlePlane &right );
};

#endif
		
		
		

		
