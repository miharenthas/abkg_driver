%-*- texinfo -*-
%@deftypefn {Function file} {[@var{run_file},@var{out_file},@var{nb_events},@var{energy},@var{beta_0}]} = upi_input_from_prompt()
%This function displays a prompt capable of reading the relevant quantities to UPI: run file, output file, number of events and energy.
%The command syntax is a) trivial b) the same as required by the config file.
%@seealso{ upi_input_from_cf, upi_parse_cmd }
%@end deftypefn

function [run_file, out_file, nb_events, nrg, beta_0 ] = upi_input_from_prompt()
	%decalre-ish the input variables
	get_out = 0;
	run_file = 'none';
	out_file = 'none';
	nb_events = 10;
	nrg = 30;
	beta_0 = 0.98;
	
	%loop on the prompt
	while ~get_out
		user_says = input( 'upi> ', 'S' );
		if ~user_says
			continue;
		end
		
		[cmd, opts] = upi_parse_cmd( user_says );
		
		switch( cmd )
			case 'beta' %get a beta for all the events
				if isempty( opts )
					disp( 'Error: beta requires one argument.' );
				else
					beta_0 = str2num( opts{1} );
				end
			case 'run' %reads a filename for the run
				if isempty( opts )
					disp( 'Error: run requires one argument.' );
				else
					run_file = opts{1};
					beta_0 = 1;
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
					nrg = [];
					for ii=1:length( opts )
						nrg = [nrg,str2num( opts{ii} )];
					end
				end
			case 'done' %exits the prompt
				get_out = 1;
			otherwise
				disp( ['"', cmd, '" is not a valid command.'] );
		end
	end
end
