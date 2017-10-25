//implementation of the make (and print, as it turns out) event toolkit

#include "p2a_make_evt.h"

namespace p2a{
	//----------------------------------------------------------------------------
	//init the event id to 0
	unsigned the_event_ID::_id = 0;
	
	//----------------------------------------------------------------------------
	//make a track out of a photon
	track photon2track( const gsl_vector *mom ){
		track trk;
		memset( &trk, 0, sizeof( track ) );
		
		trk.iPid = 1; //it's a photon, no question about it.
		trk.iA = 22; //and it's that in R3BRoot.
		trk.px = gsl_vector_get( mom, 0 );
		trk.py = gsl_vector_get( mom, 1 );
		trk.pz = gsl_vector_get( mom, 2 );
		
		return trk;
	}
	
	//----------------------------------------------------------------------------
	//make a bunch of tracks into an event
	void tracks2event( event &evt, const std::vector<track> &tracks ){
		evt.trk = tracks;
		evt.nTracks = tracks.size();
	}
	
	//----------------------------------------------------------------------------
	//produce the event header
	void make_event_hdr( event &evt, float beam_A, float beam_Z, float beam_nrg ){
		float beta = beam2beta( beam_nrg, beam_A, beam_Z );
		
		evt.eventId = ++the_event_ID::_id;
		evt.pBeam = beam2p( beam_nrg, beam_A, beam_Z );
		evt.b = beta;
	}
	
	//----------------------------------------------------------------------------
	//print the event to a stream
	void put_event( FILE *stream, event &evt ){
		fprintf( stream, "%u\t%hu\t%f\t%f\n",
		         evt.eventId,
		         evt.nTracks,
		         evt.pBeam,
		         evt.b );
		for( int i=0; i < evt.nTracks; ++i ){
			fprintf( stream, "%d\t%hhu\t%hu\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",
			         evt.trk[i].iPid,
			         evt.trk[i].iZ,
			         evt.trk[i].iA,
			         evt.trk[i].px,
			         evt.trk[i].py,
			         evt.trk[i].pz,
			         evt.trk[i].vx,
			         evt.trk[i].vy,
			         evt.trk[i].vz,
			         evt.trk[i].iMass );
		}
	}
}
