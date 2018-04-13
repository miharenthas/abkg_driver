/*******************************************************
//Author: L. Zanetti (lzanetti@ikp.tu-darmstadt.de)
//Date: 26.04.2017
//Description: This class defines a giant plane detector
//             that will register energy, momentum and
//             coordinates of any incoming particle, much like
//             the rattleplane, but stops the track dead,
//             forcefully.
//Usage: Unlike other detectors, this one can (and should)
//       be placed at runtime. To do so, you have to pass an
//       instace of the rp_specs structure, carrying information
//       about the position and rotation of the plane.
//       By default, the plane will be created at the origin
//       and perpendicular to the beam (Z) axis.
//NOTE: needless to say, this is a probe for the simulation
//      not a real detector.
*******************************************************/

#ifndef R3BSTOPPERPLANE__H
#define R3BSTOPPERPLANE__H

//define a constant for the detector ID
#define STOPPERPLANE_DETECTOR_ID kSTOPPERPLANE

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
#include "R3BRattlePlane.h" //The rattleplane, from which this
                             //inherits
#include "R3BRPHit.h" //the rattle of the rattle plane, used by this as well.
#include "R3BTargetAssemblyMedia.h" //This is here to provide materials, even
                                    //if this is not strictly its intended
                                    //application. Still, should work.

class R3BStopperPlane : public R3BRattlePlane {
	public:
		//NOTE: the "handy structure" rp_specs
		//      should be inherited as well
		
		//ctors, dtor.
		//As mentioned above, if not default constructed, this class
		//should be given an affine transformation to place the plane.
		R3BStopperPlane();
		//parametric constructor:
		// -- specs: the spect structure containig the measures and
		//           placement of the rattleplane.
		//           NOTE: there seem to be a slight problem with the
		//                 rotations (02.03.2017)
		// -- name: a string to name the thing. "Nebuchadnezzar" is the default.
		// -- active: a flag to switch on and off the detector.
		R3BStopperPlane( rp_specs &specs, const char *the_name, bool active );
		
		virtual ~R3BStopperPlane(){};
		
		//The only method that should require rewriting is the ProcessHit one
		//because we need to kill the track in its steps.
		virtual Bool_t ProcessHits( FairVolume *fair_vol = NULL );
		
		//usual interpreter garbage
		//NOTE: these classes don't actually work in CINT
		ClassDef( R3BStopperPlane, 1 );
	protected:
		//nothing new in the protected section
	private:
		R3BStopperPlane( const R3BStopperPlane &given ); //because of the way things are
		                                                 //passed around, it's just too much
		                                                 //trouble.
		R3BStopperPlane &operator=( R3BStopperPlane &right ); //same reasons.
};

#endif
