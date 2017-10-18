//this data structure holds the information passed by the ascii generator
//to the very-mock-indeed detector "bodgelogger" contained in this package
//it is meant to hold event ID and beam information (which R3BRoot seems not
//to care about at the moment.

#ifndef R3B_ASCII_BLOG__H
#define R3B_ACSII_BLOG__H

#include "FairMCPoint.h"

//------------------------------------------------------------------------------------
//class declaration
typedef class _r3b_ascii_blog : public FairMCPoint {
	public:
		_r3b_ascii_blog() {};
		virtual ~_r3b_ascii_blog() {};
		
		unsigned event_id;
		unsigned nb_tracks;
		float beam_momentum;
		float beam_impact_parameter;
		//TODO: more to come
		
		ClassDef( 1 );
} ascii_blog;

#endif
