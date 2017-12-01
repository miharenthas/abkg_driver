//implementation of the functionZ defined here:
#include "gen_bkg_from_histo.h"

//------------------------------------------------------------------------------------
//histograms alloc/free:

//allocation
gen_bkg_hists *gen_bkg_histograms_alloc( gen_bkg_opts *go ){
	gen_bkg_hists *hists = (gen_bkg_hists*)malloc( sizeof(gen_bkg_hists) );
	
	//calculat the target length
	Double_t target_l = go->target_thickness/go->target_density; // cm
	
	//allocate the histograms:
	hists->h_int = new TH1F("h_int", "Interaction position", 1000, 0, 1.1 * target_l);
	hists->h_dist = new TH1F("h_dist", "Interaction distance from previous", 1000, 0, target_l);
	hists->h_mult = new TH1F("h_mult", "Interaction multiplicity", 1000, 0, 1000);
	hists->h_mult_g = new TH1F("h_mult_g", "Interaction multiplicity; gamma vector", 1000, 0, 1000);
	hists->h_sim = new TH2F("h_sim", "Simulated events: E vs. theta", 1000, 0, TMath::Pi(), 1000, 0, 3000);
	
	return hists;
}

//deallocation
void gen_bkg_histograms_free( gen_bkg_hists *hists ){
	//NOTE: 18.08.2017: there seem to be some new "feature" in FairRoot
	//      that causes these delete to be double-free's.
	/*delete hists->h_int;
	delete hists->h_dist;
	delete hists->h_mult;
	delete hists->h_mult_g;
	delete hists->h_sim;*/
	
	free( hists );
}

//------------------------------------------------------------------------------------
//options allocator/editor/deallocator

//allocator:
gen_bkg_opts *gen_bkg_options_alloc(){
	gen_bkg_opts *go = (gen_bkg_opts*)malloc( sizeof( gen_bkg_opts ) );
	
	//default initialization.
	//note that this is not viable because of the two
	//NULL pointers at the beginning.
	*go = {
		NULL,
		NULL,
		kFALSE,
		_GEN_BKG_DEF_EVNTS,
		_GEN_BKG_DEF_TOT_CS,
		_GEN_BKG_DEF_THICKNESS,
		_GEN_BKG_DEF_DENSITY,
		_GEN_BKG_DEF_MOLMASS,
		kFALSE,
		kFALSE
	};
	
	return go;
}

//deallocator
void gen_bkg_options_free( gen_bkg_opts *go ){
	//NOTE: I'm assuming the options are being manipulated only
	//      through the functions here, so they are properly allocated
	//this makes root 6 freak out
	/*if( go->histo != NULL && go->histo->IsOnHeap() ) go->histo->Delete();*/

	//this makes ROOT 5 freak out...
	if( ftell( go->output_target ) != -1L ){
		if( go->is_pipe ){ pclose( go->output_target ); }
		else if( go->output_target != stdout ){ fclose( go->output_target ); }
	}

	free( go );
}

//editor
void gen_bkg_options_edit( gen_bkg_opts *go, gen_bkg_fmt *format, ... ){
	//count the arguments
	int n_arg;
	for( n_arg=0; format[n_arg] != END_OF_FORMAT; ++n_arg );
	
	//init the argument list with the proper size
	va_list args; va_start( args, n_arg );
	
	for( int i=0; format[i] != END_OF_FORMAT; ++i ){
		switch( format[i] ){
			case HISTO :
				go->histo = va_arg( args, TH2* );
				break;
			case A_PIPE :
				go->output_target = va_arg( args, FILE* );
				go->is_pipe = kTRUE;
				break;
			case A_FILE :
				go->output_target = va_arg( args, FILE* );
				go->is_pipe = kFALSE;
				break;
			case EVNTS :
				go->evnts = va_arg( args, Int_t );
				break;
			case TOT_CS :
				go->tot_cs = va_arg( args, Double_t );
				break;
			case TARGET_THICKNESS :
				go->target_thickness = va_arg( args, Double_t );
				break;
			case TARGET_DENSITY :
				go->target_density = va_arg( args, Double_t );
				break;
			case TARGET_MOLMASS :
				go->target_molmass = va_arg( args, Double_t );
				break;
			case DEBUG :
				go->debug = (Bool_t)va_arg( args, Int_t );
				break;
			case VERBOSE :
				go->verbose = (Bool_t)va_arg( args, Int_t );
				break;
		}
	}
}

//------------------------------------------------------------------------------------
//format generator
gen_bkg_fmt *gen_bkg_format_alloc( const int *opt_str ){
	//count the number of options
	int n_opts = 0;
	for( int o=0; opt_str[o] != '@'; ++n_opts, ++o );
	
	//allocate the needed memory and 
	int *fmt_str = (int*)malloc( n_opts*sizeof(int) );
	memcpy( fmt_str, opt_str, n_opts*sizeof(int) );
	
	return (gen_bkg_fmt*)fmt_str;
}

//and format deallocator
void gen_bkg_format_free( gen_bkg_fmt *fmt_str ){
	free( fmt_str );
}

//------------------------------------------------------------------------------------
//The generator
void gen_bkg_from_histo( gen_bkg_opts &go ){

	//deduce from the options some relevant quantities
	//to the even generation. The names should be talking.
	Double_t target_l = go.target_thickness/go.target_density; // cm
	const Double_t barn_to_cm2 = 1.E-24;
	Double_t target_N = go.target_density * TMath::Na() / go.target_molmass; // 1/cm**3
	Double_t part_mfp = 1./(target_N * go.tot_cs * barn_to_cm2); // cm
	Double_t tar_start_offset = -0.5 * target_l; // cm

	TRandom3 rand(0); //random engine start.

	//a whole bunch of histograms to organize the data
	gen_bkg_hists *hists = gen_bkg_histograms_alloc( &go );

	//main loop, on the number of events
	if( go.verbose ) printf( "Progress: 000%\b" );
	for( int i=0; i < go.evnts; i++ ){
		if( go.verbose && !(i%100) ){
			printf( "\b\b\b" );
			printf( "%03.0f%\b", 100.*i/go.evnts );
		}

		std::vector<Double_t> pos;
		std::vector<TVector3> gamma;
		Double_t tpos = 0., last_tpos = 0.; // position in target

		//generate a chain of gammas at successive interaction points
		//do that while the calc'd interaction position is within the target length
		do {
			//get the position on target, randomly
			tpos += -TMath::Log( rand.Rndm() )*part_mfp; //tpos is the interaction position

			//if tpos is within the target length
			//then add the event to the various histograms
			if( tpos < target_l ){
				pos.push_back( tpos ); //add it to the potision vector
				hists->h_int->Fill( tpos ); //fill an histogram (absolute int position)
				hists->h_dist->Fill( tpos - last_tpos ); //fill another histogram (distance form last int)
				last_tpos = tpos; //remember for the next iteration

				//extract the data generated by ABKG from the given histogram
				Double_t theta, E;
				go.histo->GetRandom2(theta, E);

				//generate a random rotation around the beam axis
				Double_t phi = rand.Rndm()*_GEN_BKG_TAU; //directly in radians

				//debug verbosity
				if( go.debug ){
					std::cout << "\t<-DEBUG-> th/phi/E: "
					          << theta << "\t" << phi
					          << "\t" << E << std::endl;
				}

				//build the vector-direction of the emitted gamma
				TVector3 v_gam;
				v_gam.SetMagThetaPhi( E, theta*TMath::DegToRad(), phi );
				gamma.push_back( v_gam );

			}
		} while( tpos < target_l );
		
		//retireve the multiplicity of the generated gamma trail
		hists->h_mult->Fill( pos.size() );
		hists->h_mult_g->Fill( gamma.size() );
		
		//print to the output_target:
		//first line: some information about the event
		fprintf( go.output_target,
		         "%d\t%d\t0.\t0.\n",
		         i, gamma.size() );
		
		//second line: information about the gammas
		for( uint j=0; j < gamma.size(); j++ ){
			//fill an histogram
			hists->h_sim->Fill( gamma.at(j).Theta(), gamma.at(j).Mag() );
			
			//print to the output target
			//NOTE: i'm casting everything to long float a.k.a. double
			//      explicitly, in order to avoid problems.
			//NOTE: Since some time, the ASCII generator for R3B ROOT
			//      also wants the invariant mass (assuming that a variable
			//      called iMass is the invariant mass). These are photons
			//      so I'm adding a trailing 0.
			fprintf( go.output_target,
			         "1\t0\t22\t%lf\t%lf\t%lf\t0.\t0.\t%lf\t%lf\n",
			         (double)( gamma.at(j).X() * 1.E-6 ),
			         (double)( gamma.at(j).Y() * 1.E-6 ),
			         (double)( gamma.at(j).Z() * 1.E-6 ),
			         (double)( pos.at(j) + tar_start_offset ),
			         0. );
		}
	} //end of main loop.
	
	if( go.verbose ) puts( "%" );
	
	//cleanup
	gen_bkg_histograms_free( hists );
}
