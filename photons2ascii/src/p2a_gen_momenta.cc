//the implementation of the momentum toolkit

#include "p2a_gen_momenta.h"

namespace p2a{
	//----------------------------------------------------------------------------
	//most banal function ever
	gsl_vector *momentum_alloc(){
		return gsl_vector_alloc( 3 );
	}

	//----------------------------------------------------------------------------
	//second most boding function ever
	void get_momentum( gsl_vector *mom, const gsl_vector *dir, const float e ){
		//TODO: proper checking
		if( !mom || !dir ) return;
	
		mom->data[0] = e*dir->data[0];
		mom->data[1] = e*dir->data[1];
		mom->data[2] = e*dir->data[2];
	}
}
