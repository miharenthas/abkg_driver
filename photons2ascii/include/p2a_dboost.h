//doppler boost some energies acording the some angles

#ifndef P2A_DBOOST__H
#define P2A_DBOOST__H

#include <math.h>
#include <string.h>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
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
	float get_dshift( float e, float beta, const angpair &pair, const gsl_vector *beam_d );
	float get_dshift( float e, float beta, const angpair &pang, const angpair &bang );
	
	//get the photon momentum in the laboratory frame
	//the result is stored in lab_mom, which is an already allocated 4-vector
	void boost( gsl_vector *lab_mom, const gsl_vector *photon,
	            const gsl_vector *beam_d, const float beta );
	
	//conversion toolZ
	//all vectors are supposed to be preallocated
	void mom2fourmom( gsl_vector *fourm, const gsl_vector *threem, const float rest_mass );
	float fourmom2mom( gsl_vector *threem, const gsl_vector *fourm ); 
	
	//and a tool that is handy
	float beam2p( float beam_e, float beam_a, float beam_z );
	float beam2beta( float beam_e, float beam_a, float beam_z );
}

#endif
