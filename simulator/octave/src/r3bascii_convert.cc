//a pair of functions that convert r3b_ascii_{track,event_alt} vectors into
//octave's structure arrays.

#include "r3bascii_convert.h"

//------------------------------------------------------------------------------------
//the routine for events
octave_map r3bascii_convert_events( std::vector<r3b_ascii_event_alt> &events ){
	unsigned int n_evt = events.size();
	dim_vector o_dim_evt( n_evt, 1 );
	
	//allocate the bits for the events
	Cell o_field_eventId( o_dim_evt );
	Cell o_field_nTracks( o_dim_evt );
	Cell o_field_pBeam( o_dim_evt );
	Cell o_field_b( o_dim_evt );
	Cell o_field_first_track( o_dim_evt );
	
	//copy the events
	unsigned int current_first_track = 1; //this equals to one because I'm importing
	                                      //data in octave
	for( int e=0; e < n_evt; ++e ){
		o_field_eventId(e) = events[e].eventId;
		o_field_nTracks(e) = events[e].nTracks;
		o_field_pBeam(e) = events[e].pBeam;
		o_field_b(e) = events[e].b;
		o_field_first_track(e) = current_first_track;
		
		//update the first track
		current_first_track += events[e].nTracks;
	}
	
	//make the structure
	octave_map o_map_events;
	o_map_events.setfield( "eventId", o_field_eventId ); o_field_eventId.clear();
	o_map_events.setfield( "nTracks", o_field_nTracks ); o_field_nTracks.clear();
	o_map_events.setfield( "pBeam", o_field_pBeam ); o_field_pBeam.clear();
	o_map_events.setfield( "b", o_field_b ); o_field_b.clear();
	o_map_events.setfield( "first_track", o_field_first_track ); o_field_first_track.clear();
	
	return o_map_events;
}

//------------------------------------------------------------------------------------
//the routine for the tracks.
octave_map r3bascii_convert_tracks( std::vector<r3b_ascii_track> &tracks ){
	unsigned int n_trk = tracks.size();
	dim_vector o_dim_trk( n_trk, 1 );
	
	//allocate the bits of the track
	Cell o_field_iPid( o_dim_trk );
	Cell o_field_iA( o_dim_trk );
	Cell o_field_iZ( o_dim_trk );
	Cell o_field_px( o_dim_trk );
	Cell o_field_py( o_dim_trk );
	Cell o_field_pz( o_dim_trk );
	Cell o_field_vx( o_dim_trk );
	Cell o_field_vy( o_dim_trk );
	Cell o_field_vz( o_dim_trk );
	Cell o_field_iMass( o_dim_trk );
	
	for( int t=0; t < n_trk; ++t ){
		o_field_iPid(t) = tracks[t].iPid;
		o_field_iA(t) = tracks[t].iA;
		o_field_iZ(t) = tracks[t].iZ;
		o_field_px(t) = tracks[t].px;
		o_field_py(t) = tracks[t].py;
		o_field_pz(t) = tracks[t].pz;
		o_field_vx(t) = tracks[t].vx;
		o_field_vy(t) = tracks[t].vy;
		o_field_vz(t) = tracks[t].vz;
		o_field_iMass(t) = tracks[t].iMass;
	}
	
	octave_map o_map_tracks;
	o_map_tracks.setfield( "iPid", o_field_iPid ); o_field_iPid.clear();
	o_map_tracks.setfield( "iA", o_field_iA ); o_field_iA.clear();
	o_map_tracks.setfield( "iZ", o_field_iZ ); o_field_iZ.clear();
	o_map_tracks.setfield( "px", o_field_px ); o_field_px.clear();
	o_map_tracks.setfield( "py", o_field_py ); o_field_py.clear();
	o_map_tracks.setfield( "pz", o_field_pz ); o_field_pz.clear();
	o_map_tracks.setfield( "vx", o_field_vx ); o_field_vx.clear();
	o_map_tracks.setfield( "vy", o_field_vy ); o_field_vy.clear();
	o_map_tracks.setfield( "vz", o_field_vz ); o_field_vz.clear();
	o_map_tracks.setfield( "iMass", o_field_iMass ); o_field_iMass.clear();
	
	return o_map_tracks;
}
