%This function generates an uniform distribution over a solid angle
%defined as a range of \phi and \theta.
%
% directions = pfire_distro( [theta_from, theta_to, phi_from, phi_to], number )

function directions = pfire_distro( ang, nb )
	th_from = ang(1);
	th_to = ang(2);
	ph_from = ang(3);
	ph_to = ang(4);
	
	%since there are some assumptions on this, make sure we are inside them
	%first, intervals must be in order
	if th_to < th_from error( 'Theta_from MUST be smaller than Theta_to.' ); end
	if ph_to < ph_from error( 'Phi_from MUST be smaller than Phi_to.' ); end
	if ph_from < 0 || ph_to > pi error( 'Phi MUST be in [0,pi].' ); end
	if th_from < 0 || th_to > 2*pi error( 'Theta MUST be in [0,2pi].' ); end
	

	u_rng = ( th_to - th_from )/( 2*pi );
	
	v_from = ( cos( ph_to ) +1 )/2;
	v_to = ( cos( ph_from ) +1 )/2;
	v_rng = abs( v_to - v_from );

	u_rnd = rand( nb, 1 )*u_rng + th_from/(2*pi);
	v_rnd = rand( nb, 1 )*v_rng;
	v_rnd = v_rnd + v_from;
	
	directions = [ 2*pi*u_rnd, acos(2*v_rnd - 1) + pi/2, ones( nb, 1 ) ];
	directions = sph2cart( directions );
end
	
	
