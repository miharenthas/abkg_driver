//declarations of the functions that convet vectors of structures into
//octave's structure arrays

#ifndef O_R3BASCII_CONVERT__H
#define O_R3BASCII_CONVERT__H

#include <octave/oct.h>
#include <octave/Cell.h>
#include <octave/oct-map.h>

#include <vector>

#include "r3bascii_slurp.h" //for the structures.

//kinda self explanatory here
octave_map r3bascii_convert_events( std::vector<r3b_ascii_event_alt> &events );
octave_map r3bascii_convert_tracks( std::vector<r3b_ascii_track> &tracks );

#endif
