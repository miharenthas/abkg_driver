//implementation of the bodgelogger, an injector for the _r3b_ascii_blog
//struct into the R3BBurp simulation tree

#include "r3b_ascii_bodgelogger.h"

//------------------------------------------------------------------------------------
//constructor, destructor
bodgelogger::r3b_ascii_bodgelogger():
	_blogghe( new TClonesArray( "r3b_ascii_blog", 1 ) ),
	_out_tree( NULL )
{
	inject();
}

bodgelogger::~r3b_ascii_bodgelogger(){
	delete _blogghe;
}

//------------------------------------------------------------------------------------
//clear the blogghe, ready for a new event.
void bodgelogger::clear_event_log(){
	_blogghe->Clear();
}

//------------------------------------------------------------------------------------
//receive an event log
void bodgelogger::push_event_log( const ascii_blog::entry &log ){
	_event_log = log;
	
	if( !_out_tree ) inject();
	
	(*_blogghe)[0] = new ascii_blog( _event_log );
}

//------------------------------------------------------------------------------------
//try to get the output tree
void bodgelogger::inject(){
	//inject the newly created leaf
	_out_tree = FairRootManager::Instance()->GetOutTree();
	if( _out_tree ) _out_tree->Bronch( "bodgelogger", "TClonesArray", &_blogghe );
}

ClassImp( r3b_ascii_bodgelogger );
