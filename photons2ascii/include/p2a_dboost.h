//doppler boost some energies acording the some angles

#ifndef #P2A_DBOOST__H
#define #P2A_DBOOST__H

#include <math.h>

#include <gsl/gsl_vector.h>

#include "p2a_angtools.h"

namespace p2a{
	//returns the boosted energy given an energy, a beta and an angle pair
	//the angle pair will be used to determine the inclination given the
	//beam direction
	float get_dboost( float e, float beta, angpair &pair, gsl_vector *beam_d );
	float get_dboost( float e, float beta, angpair &pang, angpair &bang );
}
