//a function to write directly maps to r3b ASCII events
//uses std streams, thus works on pipes.
//It trows an out_of_range if more tracks are requested
//than they are available.

#include "r3bascii_write.h"

void r3bascii_write( octave_map o_tracks, octave_map o_evts, FILE *out_f ){
	unsigned int track_idx = 0, evt_idx = 0;
	for( evt_idx = 0, track_idx = 0;
	    evt_idx < o_evts.length() && track_idx < o_tracks.length();
	    ++evt_idx ){
		//write the event header
		fprintf( out_f, "%u\t%hu\t%f\t%f\n",
		         (unsigned int)o_evts.getfield( "eventId" )(evt_idx).uint32_scalar_value(),
		         (unsigned short int)o_evts.getfield( "nTracks" )(evt_idx).uint16_scalar_value(),
		         o_evts.getfield( "pBeam" )(evt_idx).float_value(),
		         o_evts.getfield( "b" )(evt_idx).float_value() );
		         
		//write the tracks
		track_idx = o_evts.getfield( "first_track" )(evt_idx).uint32_scalar_value();
		--track_idx; //needed because octave indexes from 1 and normal people start from 0.
		for( int trk_internal_idx = 0;
		     trk_internal_idx < (unsigned int)o_evts.getfield( "nTracks" )(evt_idx).uint16_scalar_value();
		     ++trk_internal_idx, ++track_idx ){
			//throw an out of range if we finished the tracks.
			if( track_idx >= o_tracks.length() ){
				throw std::out_of_range( "Track out of range!" );
			}
			
			fprintf( out_f, "%d\t%hu\t%hhu\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",
			         o_tracks.getfield( "iPid" )(track_idx).int_value(),
			         (unsigned int)o_tracks.getfield( "iA" )(track_idx).uint32_scalar_value(),
			         (unsigned char)o_tracks.getfield( "iZ" )(track_idx).uint8_scalar_value(),
			         o_tracks.getfield( "px" )(track_idx).float_value(),
			         o_tracks.getfield( "py" )(track_idx).float_value(),
			         o_tracks.getfield( "pz" )(track_idx).float_value(),
			         o_tracks.getfield( "vx" )(track_idx).float_value(),
			         o_tracks.getfield( "vy" )(track_idx).float_value(),
			         o_tracks.getfield( "vz" )(track_idx).float_value(),
			         o_tracks.getfield( "iMass" )(track_idx).float_value() );
		}
	}
}
