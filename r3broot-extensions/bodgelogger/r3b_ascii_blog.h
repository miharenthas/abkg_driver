//this data structure holds the information passed by the ascii generator
//to the leaf injector "bodgelogger" contained in this package
//it is meant to hold event ID and beam information (which R3BRoot seems not
//to care about at the moment.

#ifndef R3B_ASCII_BLOG__H
#define R3B_ASCII_BLOG__H

#include "TObject.h"

//------------------------------------------------------------------------------------
//class declaration
class r3b_ascii_blog : public TObject {
	public:
		//core struct declaration
		struct entry {
			unsigned event_id;
			unsigned nb_tracks;
			float pBeam;
			float b;
			//TODO: more to come
		};
		
		r3b_ascii_blog() {};
		r3b_ascii_blog( const entry &given ):
			event_id( given.event_id ),
			nb_tracks( given.nb_tracks ),
			pBeam( given.pBeam ),
			b( given.b ) {};
		r3b_ascii_blog( const r3b_ascii_blog &given ):
			event_id( given.event_id ),
			nb_tracks( given.nb_tracks ),
			pBeam( given.pBeam ),
			b( given.b ) {};
		virtual ~r3b_ascii_blog() {};
		
		unsigned event_id;
		unsigned nb_tracks;
		float pBeam;
		float b;
		
		ClassDef( r3b_ascii_blog, 1 ); //hideous ROOT thing
};
typedef r3b_ascii_blog ascii_blog;

#endif
