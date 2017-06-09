//a function to write directly maps to r3b ASCII events
//uses std streams, thus works on pipes.
//It trows an out_of_range if more tracks are requested
//than they are available.

#include "r3bascii_write.h"

void r3bascii_write( octave_map o_tracks, octave_map o_evts, FILE *out_f ){
	unsigned int track_idx = 0, evt_idx = 0;
	
	//save ourselves the resolution of the fields for the events
	const Cell &o_evt_eventId = o_evts.getfield( "eventId" );
	const Cell &o_evt_nTracks = o_evts.getfield( "nTracks" );
	const Cell &o_evt_pBeam = o_evts.getfield( "pBeam" );
	const Cell &o_evt_b = o_evts.getfield( "b" );
	const Cell &o_evt_first_track = o_evts.getfield( "first_track" );
	
	//and same with the tracks
	const Cell &o_trk_iPid = o_tracks.getfield( "iPid" );
	const Cell &o_trk_iA = o_tracks.getfield( "iA" );
	const Cell &o_trk_iZ = o_tracks.getfield( "iZ" );
	const Cell &o_trk_px = o_tracks.getfield( "px" );
	const Cell &o_trk_py = o_tracks.getfield( "py" );
	const Cell &o_trk_pz = o_tracks.getfield( "pz" );
	const Cell &o_trk_vx = o_tracks.getfield( "vx" );
	const Cell &o_trk_vy = o_tracks.getfield( "vy" );
	const Cell &o_trk_vz = o_tracks.getfield( "vz" );
	const Cell &o_trk_iMass = o_tracks.getfield( "iMass" );

	for( evt_idx = 0, track_idx = 0;
	    evt_idx < o_evts.length() && track_idx < o_tracks.length();
	    ++evt_idx ){
		//write the event header
		fprintf( out_f, "%u\t%hu\t%f\t%f\n",
		         (unsigned int)o_evt_eventId(evt_idx).uint32_scalar_value(),
		         (unsigned short int)o_evt_nTracks(evt_idx).uint16_scalar_value(),
		         o_evt_pBeam(evt_idx).float_value(),
		         o_evt_b(evt_idx).float_value() );
		         
		//write the tracks
		track_idx = o_evt_first_track(evt_idx).uint32_scalar_value();
		--track_idx; //needed because octave indexes from 1 and normal people start from 0.
		for( int trk_internal_idx = 0;
		     trk_internal_idx < (unsigned int)o_evt_nTracks(evt_idx).uint16_scalar_value();
		     ++trk_internal_idx, ++track_idx ){
			//throw an out of range if we finished the tracks.
			if( track_idx >= o_tracks.length() ){
				throw std::out_of_range( "Track out of range!" );
			}
			
			fprintf( out_f, "%d\t%hhu\t%u\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",
			         o_trk_iPid(track_idx).int_value(),
			         (unsigned char)o_trk_iZ(track_idx).uint8_scalar_value(),
			         (unsigned int)o_trk_iA(track_idx).uint32_scalar_value(),
			         o_trk_px(track_idx).float_value(),
			         o_trk_py(track_idx).float_value(),
			         o_trk_pz(track_idx).float_value(),
			         o_trk_vx(track_idx).float_value(),
			         o_trk_vy(track_idx).float_value(),
			         o_trk_vz(track_idx).float_value(),
			         o_trk_iMass(track_idx).float_value() );
		}
	}
}
