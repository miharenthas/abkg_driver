%this function doppler shifts energies.
%it should work with arrays of energies and one sigle one.

function shifted_energies = upi_doppler_shift( nrg, beta_0, inclination )
	%precalc the sines
	%NOTE: octave stores phi as the angle from the XY plane
	%      thus this is correct.
	sn = sin( inclination );

	%do the shifting
	shifted_energies = nrg.*sqrt( ( 1 + beta_0.*sn )./( 1 - beta_0.*sn ) );
end
