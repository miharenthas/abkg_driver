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
			elseif iscell( varargin{ii+1} )
				if length( varargin{ii+1} ) ~= nb_evt
					error( '# events are mismatched.' );
				end
				field_presets{idx+1} = varargin{ii+1};
			elseif isvector( varargin{ii+1} )
				if length( varargin{ii+1} ) ~= nb_evt
					error( '# events are mismatched.' );
				end
				field_presets{idx+1} = mat2cell( varargin{ii+1}(:), ...
				                                 ones( nb_evt, 1 ), 1 );
			else
				error( 'Invalid value for a field initialization.' );
			end
		end
	else
		error( 'No arguments' );
	end

	%some handy things
	init_grid = ones( nb_evt, 1 ); %an array of ones long as required
	if ~iscell( field_presets{4} )
		tpe = field_presets{4}; %tracks per event
	else
		tpe = 0;
	end
	
	%make the fields
	if ~iscell( field_presets{2} )
		eventId = mat2cell( [0:nb_evt-1]'.+field_presets{2}, init_grid, 1 );
	else
		eventId = field_presets{2};
	end
	if ~iscell( field_presets{4} )
		nTracks = mat2cell( field_presets{4}*init_grid, init_grid, 1 );
	else
		nTracks = field_presets{4};
	end
	if ~iscell( field_presets{6} )
		pBeam = mat2cell( field_presets{6}*init_grid, init_grid, 1 );
	else
		pBeam = field_presets{6}
	end
	if ~iscell( field_presets{8} )
		b = mat2cell( field_presets{8}*init_grid, init_grid, 1 );
	else
		b = field_presets{8};
	end
	if ~iscell( field_presets{10} )
		first_track = mat2cell( field_presets{10}*[1:tpe:nb_evt*tpe]', init_grid,1 );
	else
		first_track = field_presets{10};
	end
	
	%and the structure array
	evnts = struct( field_presets{1}, eventId, ...
	                field_presets{3}, nTracks, ...
	                field_presets{5}, pBeam, ...
	                field_presets{7}, b, ...
	                field_presets{9}, first_track );
end
