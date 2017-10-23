//this is a collection of generic-ish tools for manipulating angles

#ifndef P2A_ANGTOOLS__H
#define P2A_ANGTOOLS__H

#include <math.h>
#include <gsl/gsl_vector.h>

#define __d2r_conv 0.01745329251
#define __pi 3.14159265359

namespace p2a{
	//----------------------------------------------------------------------------
	//a structure to hold a pair of angles
	typedef struct _angle_pairs {
		float theta;
		float phi;
	} angpair;
	
	//----------------------------------------------------------------------------
	//allocate an aray of N angle pairs, all init to 0
	angpair *angpair_alloc( unsigned n );
	void agpair_free( angpair *angles );
	
	//----------------------------------------------------------------------------
	//since we are dealing with angles here...
	angpair deg2rad( const angpair &deg );
	angpair dir2ang( const gsl_vector *dir ); //assuming ISO sph. coordinates
	gsl_vector* ang2dir( const angpair &pair ); //same as above
	                                            //NOTE: use gsl_vector_free to
	                                            //      free the vector pointer here!
	float gcdist( const angpair &p1, const angpair &p2 ); //great circle distance
	                                                      //aka, angular distance
}

#endif
