//this function loads events from an ASCII file
//meant for use with the R3B ASCII generator
#define O_DOC_STRING "-*- texinfo -*-\n\
@deftypefn {Function File} {[@var{events}, @var{tracks}] =} r3bascii_slurp_events( @var{F_NAME}, ... )\n\
A function that quickly loads ASCII events meant to be used with the R3B ASCII.\n\
All the tracks are saved in @var{tracks}: it's a structure array with the fields:\n\
@example\n\
@group\n\
@var{tracks} :\n\
   iA -- the mass number of the particle\n\
   iZ -- the charge of the particle\n\
   px, py, pz -- components of the momentum\n\
   vx, vy, vz -- components of the velocity\n\
   iMass -- the invariant mass of the particle\n\
@end group\n\
@end example\n\
The events are memorized as logical, in a structure array with the fields:\n\
@example\n\
@group\n\
@var{events} :\n\
   eventId -- the ID of the events (usually, a progressive number)\n\
   nTracks -- number of tracks in the current event\n\
   pBeam --\n\
   b --\n\
   first_track -- the index of the first track found in @var{tracks}.\n\
@end group\n\
@end example\n\
@end deftypefn"

#include <octave/oct.h>
#include <octave/oct-map.h>

#include <stdio.h>
#include <stdlib.h>

#include <vector>

#include "r3bascii_slurp.h"
#include "r3bascii_convert.h"

DEFUN_DLD( r3bascii_slurp_events, args, , O_DOC_STRING ){
	unsigned int nargin = args.length();

	FILE **infile = (FILE**)malloc( sizeof(FILE*) );

	//open the various files:
	//if we have only one string argument, assume it's a file and
	//try to open it. If we have just a bunch of strings, open a
	//corresponding bunch of files.
	if( nargin == 1 && args(0).is_string() ){
		infile[0] = fopen( args(0).string_value().c_str(), "r" );
		if( infile[0] == NULL )
			error( "Can't open the file \"%s\".", args(0).string_value().c_str() );
	} else if( nargin > 1 ){
		infile = (FILE**)realloc( infile, nargin*sizeof(FILE*) );
		for( int i=0; i < nargin; ++i ){
			infile[i] = fopen( args(i).string_value().c_str(), "r" );
			if( infile[i] == NULL )
				error( "Can't open the file \"%s\".", args(i).string_value().c_str() );
		}
	} else error( "A file name is required." );
	
	//if we survived until here, let's load
	void **et = r3bascii_slurp( infile, nargin );
	for( int i=0; i < nargin; ++i ) fclose( infile[i] );
	
	//linearize and convert as two arrays, one that carries information
	//about the event, the other about the tracks.
	octave_value_list o_data;
	o_data.append( r3bascii_convert_events( *(std::vector<r3b_ascii_event_alt>*)et[0] ) );
	o_data.append( r3bascii_convert_tracks( *(std::vector<r3b_ascii_track>*)et[1] ) );
	
	//free the buffers.
	r3bascii_slurp_free( et );
	
	return o_data;
}
	
	
		
