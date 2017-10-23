//these tools assist you into the creation of momenta

#ifndef P2A_GEN_MOMENTA__H
#define P2A_GEN_MOMENTA__H

#include <gsl/gsl_vector.h>

namespace p2a{
	gsl_vector *momentum_alloc(); //use gsl_vector_free to free the momentum vector!

	//multiply a direction by an energy.
	void get_momentum( gsl_vector *mom, const gsl_vector *dir, const float e );
}

#endif

