%this function takes in directions and turns them into 3-momenta.
%
% momenta = pfire_momenta( directions, energy )
%
%NOTE: units is GeV/c*

function momenta = pfire_momenta( directions, PDG, energy )
	mom_mod = sqrt( 2*energy*PDG.mass );
	momenta = directions*mom_mod;
end

%ridiculous function...
