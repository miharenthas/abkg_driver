%-*- texinfo -*-
%@deftypefn {Script} particle_firehose()
%@deftypefnx {Script} particle_firehose( @var{config_file} ) %%coming soon.
%@deftypefnx {Script} particle_firehose( @var{PDG}, @var{energy},
%                                        @var{solid_angle}, @var{event_specs},
%                                        @var{out_file} )
%
%This scripts allows to fire bunches of particle in a specified solid angle with a definite energy or range of energies. The particles are generated uniformly distributed in the intervals, if they are intervals, or monochromatic.
%@var{PDG}: defines the kind(s) of particles. It's a structure (with also the MASS in it).
%@var{energy}: if scalar, defines the energy; if a vector, defines the range.
%@var{solid_angle}: defines the solid angle to fire particles at. It's a square region.
%@var{event_specs}: contains the event specification: how many particle per kind and so on.
%
%This script requires the interface provided with the abkg_progs repository -of which this script is part.

function particle_firehose( varargin )
	disp( 'Welcome int PFIRE - fire arbitrary particles at a solid angle!' );
	
	if ~nargin
		#input from prompt
		[PDG, energy, solid_angle, event_specs, out_file] = ...
			pfire_input_from_prompt();
	elseif nargin == 1
		#input from config file
		[PDG, energy, solid_angle, event_specs, out_file] = ...
			pfire_input_from_cf();
	elseif nargin == 5
		PDG = varargin{1};
		energy = varargin{2};
		solid_angle = varargin{3};
		event_specs = varargin{4};
		out_file = varargin{5};
	end
	
	nb_tracks = event_specs.trk_p_evt*event_specs.nb_events;
	
	%generate the uniform distributions
	directions = pfire_distro( solid_angle, nb_tracks );
	momenta = pfire_momenta( directions, PDG, energy );

	%make the events' bits
	evts = r3bascii_evnts_alloc( event_specs.nb_events, 'nTracks', event_specs.trk_p_evt );
	%ionZ and other particles are saved differently as a track
	%se we have to differentiate.
	if PDG.pdg >= 1000010020
		trks = r3bascii_tracks_alloc( nb_tracks, 'iPid', -1,  ...
		                              'iA', PDG.A, 'iZ', PDG.Z, 'px', momenta'(1,:), ...
		                              'py', momenta'(2,:), 'pz', momenta'(3,:), ...
		                              'iMass', PDG.mass );
	else
		trks = r3bascii_tracks_alloc( nb_tracks, 'iPid', 1,  ...
		                              'iA', PDG.pdg, 'iZ', 0, 'px', momenta'(1,:), ...
		                              'py', momenta'(2,:), 'pz', momenta'(3,:), ...
		                              'iMass', PDG.mass );
	end
	
	%write the events
	r3bascii_write_compressed_events( trks, evts, out_file );
end
