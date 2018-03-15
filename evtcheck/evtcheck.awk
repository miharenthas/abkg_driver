#! /bin/awk -f
#this script checks the R3B ASCII events from a generator for obvious inconsistencies.
BEGIN { 
	beamsq = 65536; exp_trk = 0; trk = 0; e_id = 0;
	nb_events = 0; nb_tracks = 0;
	nb_mismatch_trk = 0;
	nb_nonstandard = 0;
	nb_weirdmom = 0;
}
{
	if( NF == 4 ){
		nb_events++;
		if( trk < exp_trk ){ print "event ", e_id, " has less tracks than expected!"; nb_mismatch_trk++; }
		e_id = $1; exp_trk = $2; beamsq = $3*$3;
		if( beamsq == 0 ) beamsq = 65536; #GeV, total E_k of the beam, speculated (usually: ~143)
		trk = 0;
	} else if( NF == 10 ){
		trk++; nb_tracks++;
		if( trk > exp_trk ){ print "event ", e_id, " has more tracks than expected!"; nb_mismatch_trk++; }
		if( $1 != 1 ){ print "track ", trk, " in event ", e_id, "is non-standard photon: field 1 is ", $1; nb_nonstandard++ }
		if( $2 != 0 ){ print "track ", trk, " in event ", e_id, "is non-standard photon: field 2 is ", $2; nb_nonstandard++ }
		if( $3 != 22 ){ print "track ", trk, " in event ", e_id, "is non-standard photon: field 3 is ", $3; nb_nonstandard++ }
		if( $4*$4 > beamsq || $5*$5 > beamsq || $6*$6 > beamsq ){
			print "track ", trk, " in event ", e_id, "has a suspiciously large momentum set.";
			nb_weirdmom++;
		}
	}
}
END {
	print "Events checked: ", nb_events;
	print "Number of mismatching track count: ", nb_mismatch_trk;
	print "Events with nonstandard photon: ", nb_nonstandard;
	print "Events with weird momenta: ", nb_weirdmom;
}
