//implementation of the doppler booster

#include "p2a_dboost.h"

namespace p2a{
	//----------------------------------------------------------------------------
	float get_dboost( float e, float beta, const angpair &pair, const gsl_vector *beam_d ){
		angpair bang = dir2ang( beam_d );
		get_dboost( float e, float beta, pair, bang );
	}
	
	float get_dboost( float e, float beta, const angpair &pang, const angpair &bang ){
		float inclination = gcdist( pang, bang );
		
		float gamma = 1./sqrt( 1 - pow( beta, 2 ) );
		float sni = sin( inclination );
		
		return e/( gamma*( 1 - beta*sni ) );
	}
}
