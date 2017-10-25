//this routine writes events in a r3b ASCII generator friendly manner
#define O_DOC_STRING "-*- texinfo -*-\n\
@deftypefn {Function File} r3bascii_write_events( @var{tracks}, @var{events}, @var{filename} )\n\
A function that writes events and tracks in a format compatible with R3B's ASCII generator. The file is a standard ASCII file.\n\
\n\
For more information on the format @var{tracks} and @var{events} must have, please see the documentation of the readere (slurp) funcions.\n\
@seealso{ r3bascii_slurp_events }\n\
@end deftypefn"

#include <octave/oct.h>
#include <octave/oct-map.h>

#include "r3bascii_write.h"

DEFUN_DLD( r3bascii_write_events, args, , O_DOC_STRING ){
	unsigned int nargin = args.length();
	char out_fname[512];
	
	//check the input and open the file
	if( nargin != 3 ){
		error( "Need 3 arguments: tracks, events and file name." );
	}
	
	if( nargin == 3 && args(0).is_map() && args(1).is_map() && args(2).is_string() ){
		//ok...
		strcpy( out_fname, args(2).string_value().c_str() );
	} else {
		error( "Wrong arguments. Need tracks, events and file name." );
	}
	
	//copy the maps
	octave_map o_tracks = args(0).map_value();
	octave_map o_evts = args(1).map_value();
	
	//do some more checks
	if( !o_tracks.isfield( "iPid" ) ||
	    !o_tracks.isfield( "iA" ) ||
	    !o_tracks.isfield( "iZ" ) ||
	    !o_tracks.isfield( "px" ) ||
	    !o_tracks.isfield( "py" ) ||
	    !o_tracks.isfield( "pz" ) ||
	    !o_tracks.isfield( "vx" ) ||
	    !o_tracks.isfield( "vy" ) ||
	    !o_tracks.isfield( "vz" ) ||
	    !o_tracks.isfield( "iMass" ) ){
		error( "What's been passed as tracks is missing some fields." );
	}
	
	if( !o_evts.isfield( "eventId" ) ||
	    !o_evts.isfield( "nTracks" ) ||
	    !o_evts.isfield( "pBeam" ) ||
	    !o_evts.isfield( "b" ) ||
	    !o_evts.isfield( "first_track" ) ) {
		error( "What's been passed as events is missing some fields." );
	}
	
	//open the file
	FILE* out_f = fopen( out_fname, "w" );
	if( out_f == NULL ) error( "File error." );
	
	//write down the events
	try{
		r3bascii_write( o_tracks, o_evts, out_f );
	} catch( std::exception &e ){
		fclose( out_f );
		error( "%s", e.what() );
	}

	//close the file
	fclose( out_f );
	
	return octave_value_list();
}
			         
		
