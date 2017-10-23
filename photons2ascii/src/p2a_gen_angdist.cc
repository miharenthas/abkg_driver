//implementation of the angular trickery

#include "p2a_gen_angdist.h"

namespace p2a{
	//----------------------------------------------------------------------------
	//calculate the u-v ranges from the silid angle specs
	void get_ranges( gsl_vector *ranges, fromto &fromtos ){
		if( !ranges ) return; //TODO: better check needed here
		
		float u_rng = ( fromtos.th_to - fromtos.th_from )/( 2*__pi );
		float v_from = ( cos( fromtos.ph_to ) +1 )/2;
		float v_to = ( cos( fromtos.ph_from ) +1 )/2;
		float v_rng = ( v_to - v_from );
		
		gsl_vector_set( ranges, 0, u_rng );
		gsl_vector_set( ranges, 1, v_rng );
		gsl_vector_set( ranges, 2, v_from );
		gsl_vector_set( ranges, 3, v_to );
	}
	
	//----------------------------------------------------------------------------
	//get an agle pair within the solid angle selected
	void get_randpair( angpair &pair, gsl_vector *ranges, fromto &fromtos ){
		float u_rng = gsl_vector_get( ranges, 0 );
		float v_rng = gsl_vector_get( ranges, 1 );
		float v_from = gsl_vector_get( ranges, 2 );
		
		float u_rnd = rand()*u_rng/RAND_MAX + fromtos.th_from/(2*__pi);
		float v_rnd = rand()*v_rng/RAND_MAX + v_from;
		pair.theta = 2*__pi*u_rnd;
		pair.phi = acos( 2*v_rnd -1 ) + __pi/2;
	}
	
	//----------------------------------------------------------------------------
	//get a random direction (convert from spherical to cartesian)
	void get_randdir( gsl_vector *dir, angpair &pair ){
		float s_t = sin( pair.theta );
		float c_t = cos( pair.theta );
		float s_p = sin( pair.phi );
		float c_p = cos( pair.phi );
		
		gsl_vector_set( dir, 0, s_t*c_p );
		gsl_vector_set( dir, 1, s_t*s_p );
		gsl_vector_set( dir, 2, c_t );
	}
}
