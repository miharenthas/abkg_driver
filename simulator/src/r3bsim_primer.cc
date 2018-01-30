//implementation of the function-al

#include "r3bsim_primer.h"

//------------------------------------------------------------------------------------
//some mostly useless niceities
r3bsim_primer::r3bsim_primer( const r3bsim_primer &given ):
	_g3( given._g3 ),
	_cuts( given._cuts ),
	_opts( given._opts )
{};

r3bsim_primer::r3bsim_primer( const r3bsim_g3setup &gv_g3, const r3bsim_cuts &gv_cuts, const r3bsim_opts &gv_opts ):
	_g3( gv_g3 ),
	_cuts( gv_cuts ),
	_opts( gv_opts )
{};

r3bsim_primer &r3bsim_primer::operator=( const r3bsim_primer &given ){
	_g3 = given._g3;
	_cuts = given._cuts;
	_opts = given._opts;
	
	return *this;
}

//------------------------------------------------------------------------------------
//the main thing
void r3bsim_primer::operator()(){
	//make the stack (r3bstack)
	//this will be a hideous memory leak
	//but that's the ROOT way
	//NOTE: you have to FIRST instantiate the TGeant3TGeo, which
	//      will also serve as TGeant3, to have something in gMC
	//      which is then referenced by R3BStack's constructor.
	//personal note: whoever though of this should be killed and eaten by ants.
	TGeant3TGeo *g3 = new TGeant3TGeo( "teeggeantthree" );
	R3BStack *stack = new R3BStack();
	
	stack->SetDebug( false );
	stack->StoreSecondaries( true );
	stack->SetMinPoints( 1 );
	
	g3->SetStack( stack );
	
	//the physics settings
	//repeating the comment from gconfig/g3Config.C
	/* Geant3 Phyics Settings
	* See http://hep.fi.infn.it/geant.pdf Page 188ff for more details
	*
	* These setting are (should be) equivalent to the setting in this file combined with the
	* gMC->SetProcess("name", id) calls previously used in SetCuts.C
	*/
	
	// Geant3 specific configuration for simulated Runs
	g3->SetTRIG( _g3.trig );       // Number of events to be processed
	g3->SetSWIT( _g3.swit[0], _g3.swit[1] );   // (2, 2) for debugging
	g3->SetDEBU(
		_g3.debu[0],
		_g3.debu[1],
		_g3.debu[2]
	); // (1, 100, 2) for debugging

	// Other Geant3 settings
	g3->SetAUTO( _g3.gauto ); // Select automatic STMIN etc... calc. (AUTO 1) or manual (AUTO 0)
	g3->SetABAN( _g3.aban ); // Restore 3.16 behaviour for abandoned tracks
	g3->SetOPTI( _g3.opti ); // Select optimisation level for GEANT geometry searches (0,1,2)
	g3->SetERAN( _g3.eran );

	// Cerenkov photon generation
	g3->SetCKOV( _g3.ckov ); // =1 (??) Cerenkov

	// Rayleigh effect.
	g3->SetRAYL( _g3.rayl ); // =1 Rayleigh effect.
	// =0 (Default) No Rayleigh effect.

	// Pair production.
	g3->SetPAIR( _g3.pair ); // =1 (Default) Pair production with generation of e-/e+

	// Compton scattering.
	g3->SetCOMP( _g3.comp ); // =1 (Default) Compton scattering with generation of e-

	// Photoelectric effect.
	g3->SetPHOT( _g3.phot ); // =1 (Default) Photo-electric effect with generation of the electron

	// Nuclear fission induced by a photon.
	g3->SetPFIS( _g3.pfis ); // =0 (Default) No photo-fission
	// TODO: Evaluate usage of "=1 Photo-fission with generation of secondaries"

	// delta-ray production.
	g3->SetDRAY( _g3.dray ); // =1 (Default) delty-rays production with generation of e-
	// TODO: Evaluate "=2 delta-rays production without generation of e-" instead of cutting e-

	// Positron annihilation.
	g3->SetANNI( _g3.anni ); // =1 (Default) Positron annihilation with generation of photons.

	// bremsstrahlung
	g3->SetBREM( _g3.brem ); // =1 (Default) bremsstrahlung with generation of gamma

	// Hadronic interactions.
	g3->SetHADR( _g3.hadr ); // Use a user code hadronic package: =5 GCALOR
	// =1 (Default) Hadronic interactions with generation of secondaries. (Do not use this)

	// Muon-nucleus interactions.
	g3->SetMUNU( _g3.munu ); // =1 (Default) Muon-nucleus interactions with generation of secondaries.

	// Decay in flight.
	g3->SetDCAY( _g3.dcay ); // =1 (Default) Decay in flight with generation of secondaries

	// Continuous energy loss.
	g3->SetLOSS( _g3.loss ); // =1 Continuous energy loss with generation of δ-rays above DCUTE (common /GCUTS/) and
						// restricted Landau fluctuations below DCUTE. (=3 Same as 1, kept for backward compatibility)

	// Multiple scattering.
	g3->SetMULS( _g3.muls ); // =1 (Default) Multiple scattering according to Moliere theory

	// Collision sampling method to simulate energy loss in thin material
	g3->SetSTRA( _g3.stra ); // =1 Collision sampling activated.
	// =0 (Default) Collision sampling switched off.
	
	//and now, the SetCuts
	gMC->SetCut("CUTGAM", _cuts.gamma );   /** gammas (GeV)*/
	gMC->SetCut("CUTELE", _cuts.electron );   /** electrons (GeV)*/
	gMC->SetCut("CUTNEU", _cuts.neutral_had );   /** neutral hadrons (GeV)*/
	gMC->SetCut("CUTHAD", _cuts.charged_had );   /** charged hadrons (GeV)*/
	gMC->SetCut("CUTMUO", _cuts.muon );   /** muons (GeV)*/
	gMC->SetCut("BCUTE", _cuts.electron_bremss );    /** electron bremsstrahlung (GeV)*/
	gMC->SetCut("BCUTM", _cuts.muon_bremss );    /** muon and hadron bremsstrahlung(GeV)*/
	gMC->SetCut("DCUTE", _cuts.electron_delta );    /** delta-rays by electrons (GeV)*/
	gMC->SetCut("DCUTM", _cuts.muon_delta );    /** delta-rays by muons (GeV)*/
	gMC->SetCut("PPCUTM", _cuts.muon_pair );   /** direct pair production by muons (GeV)*/
	gMC->SetCut("TOFMAX", _cuts.max_ToF ); /**time of flight cut in seconds*/
}
