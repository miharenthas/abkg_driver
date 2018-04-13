//this is a very-mock-indeed detector that communcates directly with the custom
//ascii event generator provided with this package.
//it's purpose is to save relevant event information that doesn't make it through R3BRoot
//such as the event ID (?) and beam parameters

#ifndef R3B_ASCII_BODGELOGGER__H
#define R3B_ASCII_BODGELOGGER__H

#include "TClonesArray.h"
#include "FairRootManager.h"

#include "r3b_ascii_blog.h"

//------------------------------------------------------------------------------------
//class declaration of the bodgelogger
typedef class r3b_ascii_bodgelogger {
	public:
		r3b_ascii_bodgelogger();
		~r3b_ascii_bodgelogger();

		void push_event_log( const ascii_blog::entry &log );
		void clear_event_log();
		
	private:
		ascii_blog::entry _event_log;
		
		TClonesArray *_blogghe;
		TTree *_out_tree;
		void inject();
	public:
		ClassDef( r3b_ascii_bodgelogger, 1 );
} bodgelogger;

#endif

