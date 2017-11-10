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
	
	angpair get_dangle( float beta, const angpair &pang, const angpair &bang ){
		float inclination = gcdist( pang, bang );
		float C_o = (cos( inclination ) - beta)/(1- beta*cos( inclination ));
		
		gsl_vector *pdir = ang2dir( pang );
		gsl_vector *bdir = ang2dir( bang );
		gsl_vector *x = gsl_vector_alloc( 3 );
		
		//solve the system:
		//cos( inc_o ) = ( x, bdir )
		//x = u*pdir + v*bdir
		//v can be set and obtaina valid x
		//and the direction is set, who cares about the norm.
		double bp; gsl_blas_ddot( bdir, pdir, &bp );
		gsl_blas_dcopy( pdir, x );
		gsl_blas_daxpy( C_o - bp, bdir, x );
		
		printf( "%f %f %f, %f %f %f\n", pdir->data[0], pdir->data[1], pdir->data[2], x->data[0], x->data[1], x->data[2] );
		
		gsl_vector_free( pdir );
		gsl_vector_free( bdir );
		
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
