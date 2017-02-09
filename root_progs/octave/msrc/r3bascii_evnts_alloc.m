%This function allocates a number of events, not pointing to any track yet
%The actual track association will happen elsewhere.

function evnts = r3bascii_evnts_alloc( varargin )
	field_presets = { ...
		'eventId', 0, ...
		'nTracks', 1, ...
		'pBeam', 0, ...
		'b', 0, ...
		'first_track', 1, ...
	};

	%some input parsing
	if nargin == 1
		nb_evt = varargin{1};
	elseif nargin > 1
		nb_evt = varargin{1}; %varargin{1} is always the number of events
		%as for the rest, they are interleaved: 'field_name', init value.
		for ii=2:2:nargin
			if ischar( varargin{ii} )
				idx = find( strcmp( field_presets, varargin{ii} ) );
				if ~isscalar( idx )
					error( 'Invalid field name' );
				end
			else
				error( 'Invalid field name' );
			end
			
			if isscalar( varargin{ii+1} )
				field_presets(idx+1) = varargin{ii+1};
			else
				error( 'Invalid value for a field initialization.' );
			end
		end
	else
		error( 'No arguments' );
	end

	%some handy things
	init_grid = ones( nb_evt, 1 ); %an array of ones long as required
	tpe = field_presets{4}; %tracks per event
	
	%make the fields
	eventId = mat2cell( [0:nb_evt-1]'.+field_presets{2}, init_grid, 1 );
	nTracks = mat2cell( field_presets{4}*init_grid, init_grid, 1 );
	pBeam = mat2cell( field_presets{6}*init_grid, init_grid, 1 );
	b = mat2cell( field_presets{8}*init_grid, init_grid, 1 );
	first_track = mat2cell( field_presets{10}*[1:tpe:nb_evt*tpe]', init_grid,1 );
	
	%and the structure array
	evnts = struct( field_presets{1}, eventId, ...
	                field_presets{3}, nTracks, ...
	                field_presets{5}, pBeam, ...
	                field_presets{7}, b, ...
	                field_presets{9}, first_track );
end
