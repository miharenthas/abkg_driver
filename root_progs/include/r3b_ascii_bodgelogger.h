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
typedef _r3b_ascii_bodgelogger {
	public:
		_r3b_ascii_bodgelogger();
		~_r3b_ascii_bodgelogger();

		void push_event_log( const ascii_blog &log );
		void clear_event_log();
		
	private:
		ascii_blog _event_log;
		
		TClonesArray *_blogghe;
	
		ClassDef( bodgelogger, 0 );
} bodgelogger;

#endif

