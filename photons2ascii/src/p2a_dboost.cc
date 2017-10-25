//implementation of the doppler booster

#include "p2a_dboost.h"

namespace p2a{
	//----------------------------------------------------------------------------
	//the doppler booster itself
	float get_dboost( float e, float beta, const angpair &pair, const gsl_vector *beam_d ){
		angpair bang = dir2ang( beam_d );
		get_dboost( e, beta, pair, bang );
	}
	
	float get_dboost( float e, float beta, const angpair &pang, const angpair &bang ){
		float inclination = gcdist( pang, bang );
		
		float gamma = 1./sqrt( 1 - pow( beta, 2 ) );
		float sni = sin( inclination );
		
		return e/( gamma*( 1 - beta*sni ) );
	}
	
	//----------------------------------------------------------------------------
	//convert beam parameters to beam momentum
	float beam2p( float beam_e, float beam_a, float beam_z ){
		float rest_m = __proton_mass*beam_z + __neutron_mass*( beam_a - beam_z );
		float e = beam_e + rest_m;
		return sqrt( pow( e, 2 ) - pow( rest_m, 2 ) );
	}
	
	//----------------------------------------------------------------------------
	//convert beam parameters to beta
	float beam2beta( float beam_e, float beam_a, float beam_z ){
		float rest_m = __proton_mass*beam_z + __neutron_mass*( beam_a - beam_z );
		float e = beam_e + rest_m;
		float p = sqrt( pow( e, 2 ) - pow( rest_m, 2 ) );
		return p/e;
	}
}
