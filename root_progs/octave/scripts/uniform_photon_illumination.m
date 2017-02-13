%-*- texinfo -*-
%@deftypefn {Script} uniform_photon_illumination()
%@deftypefnx {Script} uniform_photon_illumination( @var{config_file} )
%@deftypefnx {Script} uniform_photon_illumination( @var{run_file}, @var{output_file}, @var{nb_events}, @var{energy} )
%
%This script generates a uniform, monochromaic single photon illumination, which is also doppler shifted, to be used as R3B ASCII events. The main purpose is to verify that the Crystalball data are processed well.
%
%The script can be called without arguments, in which case a prompt is provided to instruct the generation, with a config file or with four arguments representing the file containing the track information for the target run @var{run_file}, the name of the output file @var{output_file}, the number of events @var{nb_events} and the @var{energy} with wich the photons are generated.
%
%This script requires the interface provided with the abkg_progs repository -of which this script is part.
%
%@seealso{upi_doppler_shift, upi_gen_momenta, upi_input_from_cf, upi_input_from_prompt, upi_parse_cmd}
%@end deftypefn

%this is basically the program's main.
function uniform_photon_illumination( varargin )
	disp( 'Welcome in UPI - uniform photon illumination!' );
	%keep track of the time
	init_t = time;
	
	if ~nargin
		%input from prompt
		[run_file, out_file, nb_events, nrg ] = upi_input_from_prompt();
	elseif nargin == 1
		if ischar( varargin{1} )
			%load a config file with the settings
			upi_input_from_cf( varargin{1} );
		else
			error( 'One agument given: I expected a path to a config file.' );
		end
	elseif nargin == 4
		run_file = varargin{1}; %file where the run is to be found
		out_file = varargin{2}; %where we're going to save th events
		nb_events = varargin{3}; %number of events to be generated
		nrg = varargin{4}; %the energy of the photons.
	end
	
	disp( 'UPI will run with:' );
	printf( ['\trun file: ',run_file,'\n'] );
	printf( ['\toutput file: ',out_file,'\n'] );
	printf( ['\t# events: ',num2str(nb_events),'\n'] );
	printf( ['\tenergy (KeV): ',num2str(nrg),'\n\n'] );
	

	%first job: load the run to get the doppler shift
	printf( 'Loading track info...' );
	run_track_info = xb_load_track_info( run_file );
	beta_0 = [run_track_info.beta_0];
	clear run_track_info; %some pointless cleanup
	printf( ' done.\n' );
	
	%timing checkpoint
	elapsed_t = time - init_t;
	disp( ['Elapsed time: ',num2str( elapsed_t ),' s.'] );
	
	%shuffle beta_0
	printf( 'Preparing events...' );
	%tailor beta_0 to the number of events
	if length( beta_0 ) >= nb_events
		beta_0 = beta_0(1:nb_events);
	else
		%duplicate it as many times as necessary
		while length( beta_0 ) < nb_events
			beta_0 = [beta_0(:)',beta_0(:)'];
		end
		beta_0 = beta_0(1:nb_events);
	end
	
	%and shuffle
	beta_0 = beta_0( randperm( nb_events ) );		
	
	printf( ' done.\n' );
	
	%timing checkpoint
	elapsed_t = time - init_t;
	disp( ['Elapsed time: ',num2str( elapsed_t ),' s.'] );
	
	%get the photon's momenta
	printf( 'Generating uniform illumination...' );
	momenta = upi_gen_momenta( nrg, beta_0, nb_events );
	
	%remember that r3broot works in GeV, so a conversion is needed
	momenta *= 1e-6;
	
	%make the events and the tracks
	evts = r3bascii_evnts_alloc( nb_events, 'nTracks', 1 );
	trks = r3bascii_tracks_alloc( nb_events, 'iPid', 1, 'iA', 0, 'iZ', 22, ...
	                              'px', momenta(1,:), ...
	                              'py', momenta(2,:), ...
	                              'pz', momenta(3,:) );
	printf( ' done.\n' );
	
	%timing checkpoint
	elapsed_t = time - init_t;
	disp( ['Elapsed time: ',num2str( elapsed_t ),' s.'] );
	
	%finally, write the events
	printf( 'Saving...' );
	r3bascii_write_compressed_events( trks, evts, out_file );
	printf( ' done.\n' );
	
	%timing checkpoint
	elapsed_t = time - init_t;
	disp( ['Elapsed time: ',num2str( elapsed_t ),' s.'] );
	
	disp( 'Completed. Goodbye.' );
end
	
	
		
		
