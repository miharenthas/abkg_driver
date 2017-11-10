//doppler boost some energies acording the some angles

#ifndef P2A_DBOOST__H
#define P2A_DBOOST__H

#include <math.h>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>

#include "p2a_angtools.h"

#ifndef __proton_mass
#define __proton_mass 0.9382720814 //GeV/c^2
#endif
#ifndef __neutron_mass
#define __neutron_mass 0.9395654135 //GeV/c^2
#endif

namespace p2a{
	//returns the boosted energy given an energy, a beta and an angle pair
	//the angle pair will be used to determine the inclination given the
	//beam direction
	float get_dboost( float e, float beta, const angpair &pair, const gsl_vector *beam_d );
	float get_dboost( float e, float beta, const angpair &pang, const angpair &bang );
	
	//boost the emission angle
	angpair get_dangle( float beta, const angpair &pair, const gsl_vector *beam_d );
	angpair get_dangle( float beta, const angpair &pang, const angpair &bang );
	
	//and a tool that is handy
	float beam2p( float beam_e, float beam_a, float beam_z );
	float beam2beta( float beam_e, float beam_a, float beam_z );
}

#endif
