//implementation of the customized ascii generator
#include "r3b_ascii_gen.h"

//------------------------------------------------------------------------------------
//constructors:

//named file (or pipe):
r3b_ascii_gen::_r3b_ascii_generator_custom( bool verbose ):
	_verbose( verbose ),
	fPDG( TDatabasePDG::Instance() ),
	fPointVtxIsSet( kFALSE ),
	fBoxVtxIsSet( kFALSE )
{
	//retrieve the upsream simulation runner
	_parent_runner = FairRunSim::Instance();	
}

//copy:
r3b_ascii_gen::_r3b_ascii_generator_custom( const r3b_ascii_gen &given ):
	_verbose( given._verbose ),
	fX( given.fX ),
	fY( given.fY ),
	fZ( given.fZ ),
	fDX( given.fDX ),
	fDY( given.fDY ),
	fDZ( given.fDZ ),
	fPointVtxIsSet( given.fPointVtxIsSet ),
	fBoxVtxIsSet( given.fBoxVtxIsSet )
{
	//clone the fPDG (does it really make sense? Shall I do it static)
	fPDG = TDatabasePDG::Instance();
	
	//retrieve the upsream simulation runner
	_parent_runner = FairRunSim::Instance();
}

//------------------------------------------------------------------------------------
//assignment operator
r3b_ascii_gen &r3b_ascii_gen::operator=( const r3b_ascii_gen &right ){
	_verbose = right._verbose;
	fX = right.fX;
	fY = right.fY;
	fZ = right.fZ;
	fDX = right.fDX;
	fDY = right.fDY;
	fDZ = right.fDZ;
	fPointVtxIsSet = right.fPointVtxIsSet;
	fBoxVtxIsSet = right.fBoxVtxIsSet;
	fIonMap = right.fIonMap;

	//clone the fPDG (does it really make sense? Shall I do it static)
	fPDG = TDatabasePDG::Instance();
	
	//retrieve the upsream simulation runner
	_parent_runner = FairRunSim::Instance();
}

//-----------------------------------------------------------------------------------
//slurp data and store them
unsigned int r3b_ascii_gen::slurp( FILE *input_target ){
	unsigned int so_many = 0;
	--so_many; //make it wrap around (2^32-1 events max; memory will run out much sooner)
	return slurp( input_target, so_many );
}

//this is the full banana implementation
//it also registers the ionZ
unsigned int r3b_ascii_gen::slurp( FILE *input_target, unsigned int so_many ){
	//declare and initialize an event holder
	r3b_ascii_event evt = {
		0,
		0,
		0.,
		0.,
		std::vector<r3b_ascii_track>( 32 )
	};
	
	if( _verbose ){
		std::cout << "-I- Slurping the events..." << std::endl;
	}
	
	//loop on the events
	Int_t pdgType = 0;
	char ion_namebuf[32];
	for( unsigned int i=0; i < so_many && !feof( input_target ) ; ++i ){
		//get the event header
		fscanf( input_target, "%u\t%hu\t%f\t%f", &evt.eventId,
		        &evt.nTracks, &evt.pBeam, &evt.b );
		
		if( _verbose ){
			std::cout << "-I- R3BAsciiGenerator: Slurping Event: " << evt.eventId << ",  pBeam = "
				        << evt.pBeam << "GeV, b = " << evt.b << " fm, multiplicity " << evt.nTracks
				        << std::endl;
		}
		
		//resize the vectors if necessary
		evt.trk.resize( evt.nTracks );
		
		//loop over the tracks of the current event
		for( int t=0; t < evt.nTracks; ++t ){
			fscanf( input_target, "%d\t%hhu\t%hu\t%f\t%f\t%f\t%f\t%f\t%f\t%f",
				&evt.trk[t].iPid, &evt.trk[t].iZ, &evt.trk[t].iA, &evt.trk[t].px, &evt.trk[t].py,
				&evt.trk[t].pz, &evt.trk[t].vx, &evt.trk[t].vy, &evt.trk[t].vz, &evt.trk[t].iMass );
			
			Int_t pdgType = 0;
		
			//if it's an ion, register it (old registerIon)
			if( evt.trk[t].iPid < 0 ){
				if( evt.trk[t].iZ == 1 && evt.trk[t].iA == 2 ) strcpy( ion_namebuf, "Deuteron" );
				else sprintf( ion_namebuf, "Ion_%hu_%hhu", evt.trk[t].iA, evt.trk[t].iZ );
				if( fIonMap.find( ion_namebuf ) == fIonMap.end() ){ //new ion, register it
					fIonMap[ion_namebuf] = 1;
					_parent_runner->AddNewIon( new FairIon( ion_namebuf,
					                                        evt.trk[t].iZ,
					                                        evt.trk[t].iA,
					                                        evt.trk[t].iZ,
					                                        0.,
					                                        evt.trk[t].iMass ) );
				}
			}
		
			//enrich the information of the track
			if( fPointVtxIsSet ){ 
				evt.trk[t].vx = fX;
				evt.trk[t].vy = fY;
				evt.trk[t].vz = fZ;
				if( fBoxVtxIsSet ){
					evt.trk[t].vx = gRandom->Gaus( fX, fDX );
					evt.trk[t].vy = gRandom->Gaus( fY, fDY ); 
					evt.trk[t].vz = gRandom->Gaus( fZ, fDZ ); 
				}
			}
		} //end of track loop
		
		//push the event
		//NOTE: this will be slow on many events (not to mention very memory
		//      intensive. Smarter things are coming
		//TODO: implement a smarter way to deal with the situation.
		_event_buf.push( evt );
	} //end of the event loop

	if( _verbose ) std::cout << "-I- EOF reached. Bye." << std::endl;
	
	return _event_buf.size();
}
		
//------------------------------------------------------------------------------------
//extraxt the events from the huge buffer
Bool_t r3b_ascii_gen::ReadEvent( FairPrimaryGenerator *fpg ){
	char ion_namebuf[32];
	TParticlePDG *p_part;
	Int_t pdg_code = 0;
	
	if( !_event_buf.size() ){
		if( _verbose ) std::cout << "-I- EOF reached. Bye." << std::endl;
		return kFALSE;
	}
	
	if( _verbose ){
		std::cout << "-I- R3BAsciiGenerator: Giving Event: "
		          << _event_buf.front().eventId << ",  pBeam = "
			        << _event_buf.front().pBeam << "GeV, b = "
			        << _event_buf.front().b << " fm, multiplicity "
			        << _event_buf.front().nTracks << std::endl;
	}
	
	//bloop bon bthe btracks
	for( int t=0; t < _event_buf.front().nTracks; ++t ){
		if( _event_buf.front().trk[t].iPid < 0 ){ //ion
			sprintf( ion_namebuf, "Ion_%d_%d",
			         _event_buf.front().trk[t].iA,
			         _event_buf.front().trk[t].iZ );
			p_part = fPDG->GetParticle( ion_namebuf );
			//NOTE: it seems that the naming convention used in R3BAsciiGenerator
			//      is wrong. Or FairRoot's PDG database doens't know ions.
			//      Basically: this will _always_ fail, therefore the error message
			//      is printed to stderr _only_ if verbose is selected.
			if( p_part == NULL ){
					if( _verbose ) std::cerr << "-W- R3BAsciiGenerator::ReadEvent: Cannot find "
					                         << ion_namebuf << " in the PDG database!" << std::endl;
					pdg_code = 0;					
					continue;
			} else pdg_code = p_part->PdgCode();
		} else pdg_code = _event_buf.front().trk[t].iA; //not ion (just its mass number)
		
		fpg->AddTrack( pdg_code,
		               _event_buf.front().trk[t].px,
		               _event_buf.front().trk[t].py,
		               _event_buf.front().trk[t].pz,
		               _event_buf.front().trk[t].vx,
		               _event_buf.front().trk[t].vy,
		               _event_buf.front().trk[t].vz );
	} //end of track loop
	
	//remove the current event
	_event_buf.pop();
	
	//happy thoughts
	return kTRUE;
}

