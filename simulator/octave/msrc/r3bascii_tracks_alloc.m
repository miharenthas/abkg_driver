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
	elseif nargin > 1
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
			elseif iscell( varargin{ii+1} )
				if length( varargin{ii+1} ) ~= nb_tracks
					error( '# events are mismatched.' );
				end
				field_presets{idx+1} = varargin{ii+1};
			elseif isvector( varargin{ii+1} )
				if length( varargin{ii+1} ) ~= nb_tracks
					error( '# events are mismatched.' );
				end
				field_presets{idx+1} = mat2cell( varargin{ii+1}(:), ...
				                                 ones( nb_tracks, 1 ), 1 );
			else
				error( 'Invalid value for a field initialization.' );
			end
		end
	end

	%make the array struct
	init_grid = ones( nb_tracks, 1 );
	
	if ~iscell( field_presets{2} )
		iPid = mat2cell( field_presets{2}*init_grid, init_grid, 1 );
	else
		iPid = field_presets{2};
	end
	if ~iscell( field_presets{4} )
		iA = mat2cell( field_presets{4}*init_grid, init_grid, 1 );
	else
		iA = field_presets{4};
	end
	if ~iscell( field_presets{6} )
		iZ = mat2cell( field_presets{6}*init_grid, init_grid, 1 );
	else
		iZ = field_presets{6};
	end
	if ~iscell( field_presets{8} )
		px = mat2cell( field_presets{8}*init_grid, init_grid, 1 );
	else
		px = field_presets{8};
	end
	if ~iscell( field_presets{10} )
		py = mat2cell( field_presets{10}*init_grid, init_grid, 1 );
	else
		py = field_presets{10};
	end
	if ~iscell( field_presets{12} )
		pz = mat2cell( field_presets{12}*init_grid, init_grid, 1 );
	else
		pz = field_presets{12};
	end
	if ~iscell( field_presets{14} )
		vx = mat2cell( field_presets{14}*init_grid, init_grid, 1 );
	else
		vx = field_presets{14};
	end
	if ~iscell( field_presets{16} )
		vy = mat2cell( field_presets{16}*init_grid, init_grid, 1 );
	else
		vy = field_presets{16};
	end
	if ~iscell( field_presets{18} )
		vz = mat2cell( field_presets{18}*init_grid, init_grid, 1 );
	else
		vz = field_presets{18};
	end
	if ~iscell( field_presets{20} )
		iMass = mat2cell( field_presets{20}*init_grid, init_grid, 1 );
	else
		iMass = field_presets{20};
	end

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
