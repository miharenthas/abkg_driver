//implementation of the bodgelogger, an injector for the _r3b_ascii_blog
//struct into the R3BBurp simulation tree

#include "r3b_ascii_bodgelogger.h"

//------------------------------------------------------------------------------------
//constructor, destructor
bodgelogger::_r3b_ascii_bodgelogger():
	_blogghe( new TClonesArray( "ascii_blog_wrappie", 1 ) );
{
	//inject the newly created leaf
	TTree *output_tree = FairRootManager::Instance()->GetOutTree();
	output_tree->Bronch( "bodgelogger", "TClonesArray", &arr );
}

bodgelogger::~_r3b_ascii_bodgelogger(){
	delete _blogghe;
}

//------------------------------------------------------------------------------------
//clear the blogghe, ready for a new event.
bodgelogger::clear_event_log(){
	_blogghe->Clear();
}

//------------------------------------------------------------------------------------
//receive an event log
bodgelogger::push_event_log( const ascii_blog &log ){
	_event_log = log;
	
	(*_blogghe)[0] = new ascii_blog_wrappie( _event_log );
}

ClassImp( bodgelogger );
