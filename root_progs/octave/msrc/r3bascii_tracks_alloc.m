%This function allocates a number of events, not pointing to any track yet
%The actual track association will happen elsewhere.

function evnts = r3bascii_tracks_alloc( varargin )
	field_presets = { ...
		'iPid', 0, ...
		'iA', 0, ...
		'iZ', 0, ...
		'px', 0, ...
		'py', 0, ...
		'pz', 0, ...
		'vx', 0, ...
		'vy', 0, ...
		'vz', 0, ...
		'iMass', 0 ...
	};

	%some input parsing
	if nargin == 1
		nb_tracks = varargin{1};
	else if nargin > 1
		nb_tracks = varargin{1}; %varargin{1} is always the number of tracks
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
	end
	
	%make the array struct
	init_grid = ones( nb_tracks, 1 );

	iPid = mat2cell( field_presets{2}*init_grid, init_grid, 1 );
	iA = mat2cell( field_presets{4}*init_grid, init_grid, 1 );
	iZ = mat2cell( field_presets{6}*init_grid, init_grid, 1 );
	px = mat2cell( field_presets{8}*init_grid, init_grid, 1 );
	py = mat2cell( field_presets{10}*init_grid, init_grid, 1 );
	pz = mat2cell( field_presets{12}*init_grid, init_grid, 1 );
	vx = mat2cell( field_presets{14}*init_grid, init_grid, 1 );
	vy = mat2cell( field_presets{16}*init_grid, init_grid, 1 );
	vz = mat2cell( field_presets{18}*init_grid, init_grid, 1 );
	iMass = mat2cell( field_presets{20}*init_grid, init_grid, 1 );

	evnts = struct( field_presets{1}, iPid, ...
	                field_presets{3}, iA, ...
	                field_presets{5}, iZ, ...
	                field_presets{7}, px, ...
	                field_presets{9}, py, ...
	                field_presets{11}, pz, ...
	                field_presets{13}, vx, ...
	                field_presets{15}, vy, ...
	                field_presets{17}, vz, ...
	                field_presets{19}, iMass );
end
	
			
			
