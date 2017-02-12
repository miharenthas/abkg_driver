%this function parses a string into a command and arguments

function [cmd, arguments] = upi_parse_cmd( a_string )
	%first, get rid of the comments
	idx = find( a_string == '#' );
	if isscalar( idx ) && idx > 1 %it's partlty a comment
		a_string = a_string(1:idx-1);
		while a_string(end) == ' ' || a_string(end) == '\t'
			a_string = a_string(1:end-1);
		end
	elseif isscalar( idx ) && idx == 1
		cmd = [];
		arguments = {};
		return; %it's a whole commend
	end
	
	%then, tokenize the string in command and options
	cmd = [];
	arguments = {};
	[cmd, a_string] = strtok( a_string );
	
	idx = 1;
	while a_string
		[arguments(idx), a_string] = strtok( a_string );
		idx += 1;
	end
end
