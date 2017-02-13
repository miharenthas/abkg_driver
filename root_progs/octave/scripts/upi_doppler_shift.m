%-*-texinfo-*-
%@deftypefn {Function file} {@var{shifted_energies}} = upi_doppler_shift( @var{energy}, @var{beta_0}, @var{inclination} )
%This function doppler shifts energies of photons.
%
%@var{energy} is an array or a scalar representing the energy(ies) to be shifted.
%@var{beta_0} is an array or a scalar representing the incoming beta(s) of the emitting paricles.
%@var{inclination} is an array or scalar representing the angle of the photon with respect to the beam axis (usually, Z).
%@var{shifted_energies} is an array or scalar containing the doppler-shifted energy.
%@end deftypefn

function shifted_energies = upi_doppler_shift( nrg, beta_0, inclination )
	%precalc the sines
	%NOTE: octave stores phi as the angle from the XY plane
	%      thus this is correct.
	sn = sin( inclination );

	%do the shifting
	shifted_energies = nrg.*sqrt( ( 1 + beta_0.*sn )./( 1 - beta_0.*sn ) );
end
