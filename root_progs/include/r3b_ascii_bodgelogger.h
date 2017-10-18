//this is a very-mock-indeed detector that communcates directly with the custom
//ascii event generator provided with this package.
//it's purpose is to save relevant event information that doesn't make it through R3BRoot
//such as the event ID (?) and beam parameters

#ifndef R3B_ASCII_BODGELOGGER__H
#define R3B_ASCII_BODGELOGGER__H

#include "TClonesArray.h"

#include "FairLogger.h"
#include "FairRootManager.h"

#include "R3BMCStack.h" //for R3BStack
#include "R3BDetector.h" //parent class

#include "r3b_ascii_blog.h"

//------------------------------------------------------------------------------------
//class declaration of the bodgelogger
typedef _r3b_ascii_bodgelogger : public R3BDetector {
	public:
		_r3b_ascii_bodgelogger():
			R3BDetector( "Vercingetorix", true, 999 ),
			_blogghe( new TClonesArray( "_r3b_ascii_blog", 1024 ) )
		{};
		~_r3b_ascii_bodgelogger() {};

		//relevant methods
		virtual void Register();
		virtual TClonesArray *GetCollection( Int_t iColl ) const;
		virtual void EndOfEvent();
		virtual void pushLog( ascii_blog &given ); //a funny method that will be used by
		                                           //our custom ascii generator to save
		                                           //its stuff.
		
		//irrelevant methods
		virtual void Initialize() { FairDetector::Initialize(); };
		virtual Bool_t ProcessHits( FairVolume *fair_vol = NULL ) { return kTRUE; }; 
		virtual void ConstructGeometry() {};
		virtual void Print() {};
		virtual Bool_t CheckIfSensitive( std::string ){ return kTRUE; };
		
	private:
		TClonesArray *_blogghe;
		
} bodgelogger;

#endif

