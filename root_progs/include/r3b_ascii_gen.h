//this is a customized version of the R3B ASCII generator
//on top of the original implementation, it adds a verbose flag
//and it modifies the way the intput target is given and read
//this in order to allow input form pipes.
//NOTE: this code is thought to work in a "main" program, using
//      *ROOT as a library rather than as an interpreter. There's
//      absolutely no guarantee it will ever work in an interpreted
//      environment.
//***Modlist***
//1) default constructor moved to protected area
//2) private area changed into protected area
//3) "RegisterIon" method deleted in order to avoid the necessity
//   of rewinding the file (not possible with a FIFO).
//4) copy constructor reimplemented to restect ownership:
//   fIonMap and fPDG cloned, no pointer is shared.
//5) named file constructor deleted in order to not take
//   ownership of a file (open and close has to be handled
//   externally now)
//6) warning and error messages printed to STDERR
//7) added verbosity flag.
//8) destructor reimplemented in order to dispose of the object properly
//9) method "slurp" added to read the input in one big chunk
//10) protected variables modified in order to support the new method strucures


#ifndef R3B_ASCII_GEN_H
#define R3B_ASCII_GEN_H

//includes from this toolkit
//#include "r3b_ascii_paged_queue.h" //a queue-like structure that pages itself to disk
#include "r3b_ascii_event_structs.h" //the track and event data structures.

//inclusions, all of them.
#include "FairGenerator.h"
#include "FairPrimaryGenerator.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "FairIon.h"
#include "FairRunSim.h"
#include "TString.h"
#include "TRandom.h"

#include <map>
#include <vector>
#include <queue>
#include <string>

#include <stdio.h>

//some classes we need to be aware of
class TDatabasePDG;
class FairPrimaryGenerator;
class FairIon;

//------------------------------------------------------------------------------------
//the class itself
typedef class _r3b_ascii_generator_custom : public FairGenerator  
{
	public: 
		//ctors
		//the default one is private (for now) because I've found
		//a note that should not be used, so I'm inforcing that.
		_r3b_ascii_generator_custom( bool verbose = false ); //with a std stream
		_r3b_ascii_generator_custom( const _r3b_ascii_generator_custom& ); //copy
	
		//assignment operator, for whatever reason
		_r3b_ascii_generator_custom &operator=( const _r3b_ascii_generator_custom& );
		
		//dtor, reimplementable
		virtual ~_r3b_ascii_generator_custom() {};
		
		/** Reads on event from the input file and pushes the tracks onto
		** the stack. Abstract method in base class.
		** @param primGen  pointer to the R3BPrimaryGenerator
		**/
		virtual Bool_t ReadEvent( FairPrimaryGenerator* primGen );
		
		//because ions, probably, have to be registered before the simulation starts
		//this method slurps data from a pipe and stores them(*)
		//in this first implementation, it swells in RAM. Soon it will outsource
		//on disk from temporary data.
		unsigned int slurp( FILE *input_target );
		unsigned int slurp( FILE *input_target, unsigned int so_many ); //limit how many the
		                                                                //generator will read.
		
		//those two methods are used to set a bunch of coordinates
		//more on those later.
		void SetXYZ( Double32_t x=0, Double32_t y=0, Double32_t z=0 ) {
			fX=x;
			fY=y;
			fZ=z;
			fPointVtxIsSet=kTRUE;
		};

		void SetDxDyDz( Double32_t sx=0, Double32_t sy=0, Double32_t sz=0 ) {
			fDX=sx;
			fDY=sy;
			fDZ=sz;
			fBoxVtxIsSet=kTRUE;
		};

		//Get the ion name. Because of course it's not just Ion_iA_iZ,
		//but they have proper names.
		static void GetIonName( char *ion_namebuf, unsigned int iA, unsigned int iZ );
	protected: //it looks like this class could be inherited from
	           //this means that this classifier should be "potected"
		//private default constructor.
		_r3b_ascii_generator_custom():
			fPointVtxIsSet( kFALSE ),
			fBoxVtxIsSet( kFALSE ),
			_verbose( false ) {};
		
		//this is a pointer to the parent simulation runner
		//it's used to register ion tracks.
		FairRunSim *_parent_runner;
		//this is a pointer to the PDG database of the program
		TDatabasePDG *fPDG; //!  PDG database

		//in this buffer we'll store the events from
		//the input source.
		std::queue<r3b_ascii_event> _event_buf;
		
		//ion map (not vital, and probably better to save the memory)
		//but it provides an efficient way to search for keys
		//and "char" is the smallest datatype
		std::map<std::string, char> fIonMap;

		Double32_t fX, fY, fZ; // Point vertex coordinates [cm]	
		Bool_t     fPointVtxIsSet; // True if point vertex is set
		Double32_t fDX, fDY, fDZ; // Point vertex coordinates [cm] (again?)	
		Bool_t     fBoxVtxIsSet; // True if point vertex is set
		
		//verbosity flag. If true, output is printed on STDOUT
		bool _verbose;
} r3b_ascii_gen;

#endif
