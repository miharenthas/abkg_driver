//this data structure holds the information passed by the ascii generator
//to the leaf injector "bodgelogger" contained in this package
//it is meant to hold event ID and beam information (which R3BRoot seems not
//to care about at the moment.

#ifndef R3B_ASCII_BLOG__H
#define R3B_ACSII_BLOG__H

#include "TObject.h"

//------------------------------------------------------------------------------------
//core struct declaration
typedef struct _r3b_ascii_blog {
	unsigned event_id;
	unsigned nb_tracks;
	float beam_momentum;
	float beam_impact_parameter;
	//TODO: more to come
} ascii_blog;

//------------------------------------------------------------------------------------
//class declaration
typedef class _r3b_ascii_blog_wrappie : public TObject {
	public:
		_r3b_ascii_blog_wrappie() {};
		_r3b_ascii_blog_wrappie( const ascii_blog &given ):
			_blog( given ) {};
		_r3b_ascii_blog_wrappie( const _r3b_ascii_blog_wrappie &given ):
			_blog( given._blog ) {};
		virtual ~_r3b_ascii_blog() {};
		
		ascii_blog _blog = {0, 0, 0, 0};
		
		ClassDef( ascii_blog_wrappie, 0 ); //hideous ROOT thing
} ascii_blog_wrappie;

#endif
