//impelentation of the angtools

#include "p2a_angtools.h"

namespace p2a{
	//----------------------------------------------------------------------------
	//degree to radians converter
	angpair deg2rad( const angpair &deg ){
		angpair rad = { deg.theta*__d2r_conv, deg.phi*__d2r_conv };
		return rad;
	}
	
	//----------------------------------------------------------------------------
	//alloc and free angpairs
	angpair *angpair_alloc( unsigned n ){
		return (angpair*)calloc( sizeof( angpair ), n );
	}
	
	void angpair_free( angpair *angs ){
		free( angs );
	}
	
	//----------------------------------------------------------------------------
	//allocate and get a vector from an angle pair
	gsl_vector *ang2dir( const angpair &pair ){
		gsl_vector *dir = gsl_vector_alloc( 3 );
		
		float x = sin( pair.theta )*cos( pair.phi );
		float y = sin( pair.theta )*sin( pair.phi );
		float z = cos( pair.theta );
		
		gsl_vector_set( dir, 0, x );
		gsl_vector_set( dir, 1, y );
		gsl_vector_set( dir, 2, z );
		
		return dir;
	}
	
	//----------------------------------------------------------------------------
	//get an angle pair from a vector
	angpair dir2ang( const gsl_vector *dir ){
		angpair pair;
		
		float x = gsl_vector_get( dir, 0 );
		float y = gsl_vector_get( dir, 1 );
		float z = gsl_vector_get( dir, 2 );

		float r = sqrt( x*x + y*y + z*z ); //gets discarded, should be 1 anyway
		pair.theta = acos( z/r );
		pair.phi = atan2( y, x );
		
		return pair;
	}
	
	//----------------------------------------------------------------------------
	//get the great circle distance/angular distance given two angle pairs
	float gcdist( const angpair &p1, const angpair &p2 ){
		float c_1 = cos( p1.phi );
		float s_1 = sin( p1.phi );
		float c_2 = cos( p2.phi );
		float s_2 = sin( p2.phi );
		float c_D = cos( abs( p2.theta - p1.theta ) );
		float s_D = sin( abs( p2.theta - p1.theta ) );
		
		return atan2( sqrt( pow( c_2*s_D, 2 ) +
		              pow( c_1*s_2 - s_1*c_2*c_D, 2) ),
		              (s_1*s_2 + c_1*c_2*c_D) );
	}
}
