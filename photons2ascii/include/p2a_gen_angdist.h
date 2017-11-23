//this collection of functions generates a random, uniform angular distribution
//on a given solid angle (square base)
//For more, look "pfire_distro.m", which is the inspiring source for this file
//NOTE: angles are specified in radians and the naming convention follows
//      the ISO standard:
//    	        theta in [0, 2pi), longitude,
//              phi in [0, pi), latitude

#ifndef P2A_GEN_ANGDIST__H
#define P2A_GEN_ANGDIST__H

#include <math.h>
#include <time.h>
#include <stdlib.h>

#include <gsl/gsl_vector.h>

#include "p2a_angtools.h"

namespace p2a{
	typedef struct _angle_ranges {
		float ph_from;
		float ph_to;
		float th_from;
		float th_to;
	} fromto;
	//NOTE: the directions will be held in gsl_vectors
	
	//----------------------------------------------------------------------------
	//a function to get ONE random number pairs
	//ranges: a gsl_vector of length 4 to hold UV ranges
	void get_ranges( gsl_vector *ranges, fromto &fromtos );
	//NOTE: need to initialize random sequence separately (thread friendly)
	void get_randpair( angpair &pair, gsl_vector *ranges, fromto &fromtos );
	
	//----------------------------------------------------------------------------
	//get directions from the pairs
	void get_randdir( gsl_vector *dir, angpair &pair );
}

#endif
