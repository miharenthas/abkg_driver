//these tools assist you into the creation of momenta

#ifndef P2A_GEN_MOMENTA__H
#define P2A_GEN_MOMENTA__H

#include <gsl/gsl_vector.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

namespace p2a{
	gsl_vector *momentum_alloc(); //use gsl_vector_free to free the momentum vector!

	//multiply a direction by an energy.
	void get_momentum( gsl_vector *mom, const gsl_vector *dir, const float e );
	
	//a tool to get a random entry from a normal distribution centered in e and
	//with sigma sigma
	//NOTE: you MUST initialize the random generator separately
	float get_rande( float e, float sigma, gsl_rng *rng );
}

#endif

