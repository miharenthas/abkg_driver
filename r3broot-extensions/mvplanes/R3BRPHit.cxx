/*******************************************************
//Author: L. Zanetti (lzanetti@ikp.tu-darmstadt.de)
//Date: 23.02.2017
//Description: Implementation of the class
//             R3BRPHit
*******************************************************/

#include "R3BRPHit.h"

//------------------------------------------------------------------------------------
//constructors:

//nothing to see here, yet (copy and a full parametric may be implemented later)

//------------------------------------------------------------------------------------
//methods:

//------------------------------------------------------------------------------------
//The setty thing
void R3BRPHit::Finish(){
	SetMomentum( _mae.Vect() );
	SetPosition( _poe.Vect() );
}

//------------------------------------------------------------------------------------
//interpreter junk
ClassImp( R3BRPHit );




