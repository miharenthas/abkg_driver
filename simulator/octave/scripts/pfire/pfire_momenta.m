%this function takes in directions and turns them into 3-momenta.
%
% momenta = pfire_momenta( directions, energy )
%
%NOTE: units is GeV/c*

function momenta = pfire_momenta( directions, PDG, energy )
	mom_mod = sqrt( energy^2 - PDG.mass^2 );
	momenta = directions*mom_mod*1e-6;
end

%ridiculous function...
