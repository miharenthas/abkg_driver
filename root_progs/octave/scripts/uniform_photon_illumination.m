%this script creates an uniform illumination of photons
%one photon per event, a number of events specified and
%a single energy. It also doppler shifts the photons
%using a data run as a source.
%TODO: write a proper documentation. For the whole script.

%this is basically the program's main.
function uniform_photon_illumination( varargin )
	disp( 'Welcome in UPI - uniform photon illumination!' );

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
	printf( ['\tenergy: ',num2str(nrg),'\n\n'] );
	

	%first job: load the run to get the doppler shift
	printf( 'Loading track info...' );
	run_track_info = xb_load_track_info( run_file );
	beta_0 = [run_track_info.beta_0];
	clear run_track_info; %some pointless cleanup
	printf( ' done.\n' );
	
	%shuffle beta_0
	printf( 'Preparing events...' );
	%tailor beta_0 to the number of events
	if length( beta_0 ) >= nb_events
		beta_0 = beta_0(1:nb_events);
	else
		%duplicate it as many times as necessary
		while length( beta_zero ) < nb_events
			beta_0 = [beta_0(:)',beta_0(:)'];
		end
		beta_0 = beta_0(1:nb_events)
	end
	
	%and shuffle
	beta_0 = beta_0( randperm( nb_events ) );		
	
	printf( ' done.\n' );
	
	%get the photon's momenta
	printf( 'Generating uniform illumination...' );
	momenta = upi_gen_momenta( nrg, beta_0, nb_events );
	
	%remember that r3broot works in GeV, so a conversion is needed
	momenta *= 10^-6;
	
	%make the events and the tracks
	evts = r3bascii_evnts_alloc( nb_events, 'nTracks', 1 );
	trks = r3bascii_tracks_alloc( nb_events, 'iPid', 1, 'iZ', 0, 'iA', 22, ...
	                              'px', momenta(1,:), ...
	                              'py', momenta(2,:), ...
	                              'pz', momenta(3,:) );
	printf( ' done.\n' );
	
	%finally, write the events
	printf( 'Saving...' );
	r3bascii_write_compressed_events( trks, evts, out_file );
	printf( ' done.\n' );
	
	disp( 'Completed. Goodbye.' );
end
	
	
		
		
