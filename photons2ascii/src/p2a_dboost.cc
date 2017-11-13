//implementation of the doppler booster

#include "p2a_dboost.h"

namespace p2a{
	//----------------------------------------------------------------------------
	//the doppler booster itself
	float get_dshift( float e, float beta, const angpair &pair, const gsl_vector *beam_d ){
		angpair bang = dir2ang( beam_d );
		return get_dshift( e, beta, pair, bang );
	}
	
	float get_dshift( float e, float beta, const angpair &pang, const angpair &bang ){
		float inclination = gcdist( pang, bang );
		
		float gamma = 1./sqrt( 1 - pow( beta, 2 ) );
		float sni = sin( inclination );
		
		return e/( gamma*( 1 - beta*sni ) );
	}
	
	//----------------------------------------------------------------------------
	//four momenta operations
	void boost( gsl_vector *lab_mom, const gsl_vector *photon,
	            const gsl_vector *beam_d, const float beta ){
		//make the boost matrix
		gsl_matrix *B = gsl_matrix_alloc( 4, 4 );
		float gamma = 1/sqrt( 1 - pow( beta, 2 ) );
		const float *n = beam_d->data;
		
		gsl_matrix_set( B, 0, 0, gamma );
		gsl_matrix_set( B, 0, 1, -1*gamma*beta*-1*n[0] );
		gsl_matrix_set( B, 0, 2, -1*gamma*beta*-1*n[1] );
		gsl_matrix_set( B, 0, 3, -1*gamma*beta*-1*n[2] );
		gsl_matrix_set( B, 1, 0, gsl_matrix_get( B, 0, 1 ) );
		gsl_matrix_set( B, 1, 1, 1+(gamma-1)*pow( -1*n[0], 2 ) );
		gsl_matrix_set( B, 1, 2, (gamma-1)*n[0]*n[1] );
		gsl_matrix_set( B, 1, 3, (gamma-1)*n[0]*n[2] );
		gsl_matrix_set( B, 2, 0, gsl_matrix_get( B, 0, 2 ) );
		gsl_matrix_set( B, 2, 1, gsl_matrix_get( B, 1, 2 ) );
		gsl_matrix_set( B, 2, 2, 1+(gamma-1)*pow( -1*n[1], 2 ) );
		gsl_matrix_set( B, 2, 3, (gamma-1)*n[1]*n[2] );
		gsl_matrix_set( B, 3, 0, gsl_matrix_get( B, 0, 3 ) );
		gsl_matrix_set( B, 3, 1, gsl_matrix_get( B, 1, 3 ) );
		gsl_matrix_set( B, 3, 2, gsl_matrix_get( B, 2, 3 ) );
		gsl_matrix_set( B, 3, 3, 1+(gamma-1)*pow( -1*n[2], 2 ) );
		
		//make the photon's four momentum
		gsl_vector *phfm = gsl_vector_alloc( 4 );
		double phe = gsl_blas_dnrm2( photon );
		gsl_vector_set( phfm, 0, phe );
		memcpy( &phfm->data[1], photon->data, 3*sizeof(double) );
		
		//do the BOOSTing
		gsl_blas_dgemv( CblasNoTrans, 1, B, phfm, 0, lab_mom );
		
		gsl_matrix_free( B );
		gsl_vector_free( phfm );
	}
	
	//----------------------------------------------------------------------------
	//four to three momentum conversion toolsies
	void mom2fourmom( gsl_vector *fourm, const gsl_vector *threem, const float rest_mass ){
		float time_like = pow( gsl_blas_dnrm2( threem ), 2 ) - pow( rest_mass, 2 );
		gsl_vector_set( fourm, 0, time_like );
		memcpy( &fourm->data[1], threem->data, 3*sizeof(double) );
	}
	
	float fourmom2mom( gsl_vector *threem, const gsl_vector *fourm ){
		memcpy( threem->data, &fourm->data[1], 3*sizeof(double) );
		float rest_mass = pow( gsl_blas_dnrm2( threem ), 2 ) - pow( fourm->data[0], 2 );
		return rest_mass;
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
