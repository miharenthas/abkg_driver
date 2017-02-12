%A config file reader

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
					nrg = str2num( opts{1} );
				end
		end
	end
end
