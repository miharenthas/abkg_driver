%-*- texinfo -*-
%@deftypefn {Function file} {[@var{run_file},@var{out_file},@var{nb_events},@var{energy}]} = upi_input_from_cf( @var{config_file} )
%This function drives the parsing of a config file and returns the four relevant entries to the UPI script.
%The config file shall be formatted as follows:
%Everything behing an hash sign will be regarded as a comment and left out.
%Every line shall contain one single command with its arguments.
%Valid commands are: run, out, events, energy.
%The first two expect a string, the second two expect a numeric value.
%@seealso{ upi_input_from_prompt, upi_parse_cmd }
%@end deftypefn

function [run_file, out_file, nb_events, nrg ] = upi_input_from_cf( cf_name )
	%decalre-ish the input variables
	get_out = 0;
	run_file = 'none';
	out_file = 'none';
	nb_events = 10;
	nrg = 30;
	
	%check the existence of a file
	if ~exist( cf_name, 'file' )
		error( 'The config file does not exist.' );
	end

	cf = fopen( cf_name );

	%loop on the file's lines
	while ~feof( cf )
		user_says = fgetl( cf );
		if ~user_says
			continue;
		end
		
		[cmd, opts] = upi_parse_cmd( user_says );
		
		switch( cmd )
			case 'run' %reads a filename for the run
				if isempty( opts )
					disp( 'upi: syntax error: run requires one argument.' );
				else
					run_file = opts{1};
				end
			case 'out' %sets an output file name
				if isempty( opts )
					disp( 'upi: syntax error: out requires one argument.' );
				else
					out_file = opts{1};
				end
			case 'events' %sets the number of events
				if isempty( opts )
					disp( 'upi: syntax error: events requires one argument.' );
				else
					nb_events = str2num( opts{1} );
				end
			case 'energy' %sets the energy
				if isempty( opts )
					disp( 'upi: syntax error: energy requires one argument.' );
				else
					nrg = [];
					for ii=1:length( opts )
						nrg = [nrg,str2num( opts{ii} )];
					end
				end
		end
	end
end
