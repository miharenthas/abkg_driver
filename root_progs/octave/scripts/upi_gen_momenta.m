%this function generates 3-momenta starting from an energy (or an array of them)
%and an array of beta_0. The momenta are supposed to be isotropically distributed.

function momenta = upi_gen_momenta( nrg, beta_0 )
	%retrieve the number of events
	nb_events = length( beta_0 );
	
	%generate random theta-phi couples on a spehere
	uv = rand( 2, nb_events );
	th_phi = [ 2*pi.*uv(1,:) ; acos( 2.*uv(2,:) -1 ) + pi/2 ];

	%get the doppler shift and get the magnitudes
	mag = upi_doppler_shift( nrg, beta_0, th_phi(2,:) );
	
	%make the momenta and put them in the cartesian plane
	momenta = sph2cart( [th_phi;mag]' )';
end
