%-*- texinfo -*-
%@deftypefn {Function file} {[@var{PDG},@var{energy},@var{solid_angle},@var{event_specs},@var{out_file}}]} = pfire_input_from_prompt()
%This function drives the parsing of a config file and returns the four relevant entries to the PFIRE script.
%The config file shall be formatted as follows:
%Everything behing an hash sign will be regarded as a comment and left out.
%Every line shall contain one single command with its arguments.
%The first two expect a string, the second two expect a numeric value.
%@seealso{ pfire_input_from_cf, pfire_parse_cmd }
%@end deftypefn

function [ PDG, energy, solid_angle, event_specs, out_file ] = pfire_input_from_prompt()
	%decalre-ish the input variables
	get_out = 0;
	PDG.pdg = 2212;
	PDG.mass = 0.939565;
	PDG.A = 1;
	PDG.Z = 1;
	energy = 1; %GeV
	solid_angle = [0, 2*pi, 0, pi];
	event_specs.nb_events = 1e3;
	event_specs.trk_p_evt = 1;
	out_file = 'afile';
		
	%loop on the file's lines
	while ~get_out
		user_says = input( 'pfire> ', 'S' );
		if ~user_says
			continue;
		end
		
		[cmd, opts] = pfire_parse_cmd( user_says );
		
		switch( cmd )
			case 'out' %sets an output file name
				if isempty( opts )
					disp( 'pfire: syntax error: out requires one argument.' );
				else
					out_file = opts{1};
				end
			case 'events' %sets the number of events
				if isempty( opts )
					disp( 'pfire: syntax error: events requires one argument.' );
				else
					event_specs.nb_events = str2num( opts{1} );
				end
			case 'tracks' %tracks per event
				if isempty( opts )
					disp( 'pfire: syntax error: tracks requires one argument.' );
				else
					event_specs.trk_p_evt = str2num( opts{1} );
				end
			case 'energy' %sets the energy
				if isempty( opts )
					disp( 'pfire: syntax error: energy requires one argument.' );
				else
					energy = str2num( opts{1} );
				end
			case 'pdg' %sets the PDG code of the particles
				if isempty( opts )
					disp( 'pfire: syntax error: pdg requires one argument.' );
				else
					PDG.pdg = str2num( opts{1} );
				end
			case 'mass' %sets the invariant mass
				if isempty( opts )
					disp( 'pfire: syntax error: mass requires one argument.' );
				else
					PDG.mass = str2num( opts{1} );
				end
			case 'charge' %sets the charge (Z)
				if isempty( opts )
					disp( 'pfire: syntax error: charge requires one argument.' );
				else
					PDG.Z = str2num( opts{1} );
				end
			case 'A_nb' %sets the mass number (A)
				if isempty( opts )
					disp( 'pfire: syntax error: A_nb requires one argument.' );
				else
					PDG.A = str2num( opts{1} );
				end
			case 'sangle' %sets the solid angle where to fire
				if numel( opts ) ~= 4
					disp( 'pfire: syntax error: sangle requires four arguments.' );
				else
					solid_angle = zeros( 4, 1 );
					for ii=1:4
						solid_angle(ii) = str2num( opts{ii} );
					end
				end
			case 'list'
				PDG
				energy
				solid_angle
				event_specs
				out_file
			case 'done' %exits the prompt
				get_out = 1;
			otherwise
				disp( ['"', cmd, '" is not a valid command.'] );
			
		end
	end
end
