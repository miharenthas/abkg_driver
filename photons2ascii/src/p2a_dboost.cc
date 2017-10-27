//implementation of the doppler booster

#include "p2a_dboost.h"

namespace p2a{
	//----------------------------------------------------------------------------
	//the doppler booster itself
	float get_dboost( float e, float beta, const angpair &pair, const gsl_vector *beam_d ){
		angpair bang = dir2ang( beam_d );
		return get_dboost( e, beta, pair, bang );
	}
	
	float get_dboost( float e, float beta, const angpair &pang, const angpair &bang ){
		float inclination = gcdist( pang, bang );
		
		float gamma = 1./sqrt( 1 - pow( beta, 2 ) );
		float sni = sin( inclination );
		
		return e/( gamma*( 1 - beta*sni ) );
	}
	
	//----------------------------------------------------------------------------
	//the angle deviation
	angpair get_dangle( float beta, const angpair &pair, const gsl_vector *beam_d ){
		angpair bang = dir2ang( beam_d );
		return get_dangle( beta, pair, bang );
	}
	
	angpiar get_dangel( float beta, const angpair &pang, const angpair &bang ){
		float inclination = gcdist( pang, bang );
		angpair delta = { bang.theta - pang.theta, bang.phi - pang.phi };
		float inc_o = (cos( inclination ) - beta)/(1- beta*cos( inclination ));
		
		gsl_vector *pdir = ang2dir( pang );
		gsl_vector *bdir = ang2dir( bang );
		gsl_vector *x = gsl_vector_alloc( 3 );
		
		//solve the system:
		//cos( inc_o ) = < x, bdir >
		//x = u*pdir + v*bdir
		//|x| = 1
		float C = cos( inc_o );
		double pb; gls_blas_ddot( pdir, bdir, pb );
		float m = C + pb;
		float A = pb*m;
		float K = pow( m, 2 ) - 1;
		float u = (A + sqrt( pow( A, 2 ) + 4*K ))/2;
		float v = m*u;
		
		gsl_blas_dscal( u, pdir );
		gsl_blas_dscal( v, bdir );
		gsl_blas_dcpy( pdir, x );
		gsl_blas_daxpy( 1, bdir, x );
		
		return dir2ang( x );
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
