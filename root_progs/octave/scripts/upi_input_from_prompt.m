%A tiny prompt, in case we didn't bother to launch the function with
%arguments or a config file

function [run_file, out_file, nb_events, nrg ] = upi_input_from_prompt()
	%decalre-ish the input variables
	get_out = 0;
	run_file = 'none';
	out_file = 'none';
	nb_events = 10;
	nrg = 30;
	
	%loop on the prompt
	while ~get_out
		user_says = input( 'upi> ', 'S' );
		if ~user_says
			continue;
		end
		
		[cmd, opts] = upi_parse_cmd( user_says );
		
		switch( cmd )
			case 'run' %reads a filename for the run
				if isempty( opts )
					disp( 'Error: run requires one argument.' );
				else
					run_file = opts{1};
				end
			case 'out' %sets an output file name
				if isempty( opts )
					disp( 'Error: out requires one argument.' );
				else
					out_file = opts{1};
				end
			case 'events' %sets the number of events
				if isempty( opts )
					disp( 'Error: events requires one argument.' );
				else
					nb_events = str2num( opts{1} );
				end
			case 'energy' %sets the energy
				if isempty( opts )
					disp( 'Error: energy requires one argument.' );
				else
					nrg = str2num( opts{1} );
				end
			case 'done' %exits the prompt
				get_out = 1;
			otherwise
				disp( ['"', cmd, '" is not a valid command.'] );
		end
	end
end
