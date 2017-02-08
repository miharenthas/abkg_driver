//header for the r3b ASCII writer for octave
//the function writes events in r3b ASCII friendly format
//from two maps onto a standard stream --works on pipes.

#ifndef O_R3BASCII_WRITE__H
#define O_R3BASCII_WRITE__H

#include <stdio.h>

#include <octave/oct.h>
#include <octave/oct-map.h>

void r3bascii_write( octave_map o_tracks, octave_map o_evts, FILE *f_out );

#endif
