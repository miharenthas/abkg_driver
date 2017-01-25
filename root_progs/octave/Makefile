#this makefile instructs the build of the octave interface for abkg_driver

#-------------------------------------------------------------------------------------
#define some variables that can be handy here:
#environment
SRC = ./src
OCT = ./oct

#targets, flags and compiler
OBJECTS = r3bascii_convert.o r3bascii_slurp.o
OCTS = r3bascii_slurp_compressed_events r3bascii_slurp_events
OCTFILES = $(OCT)/r3bascii_slurp_compressed_events.oct $(OCT)/r3bascii_slurp_events.oct

OCTFLAGS = -I"include/"
CFALGS = -I"include/"

C_OCT = mkoctfile
CXX = g++

#-------------------------------------------------------------------------------------
#recipes:

#-------------------------------------------------------------------------------------
#object files
r3bascii_convert.o :
	$(C_OCT) $(OCTFLAGS) $(SRC)/r3bascii_convert.cc -fPIC -c

r3bascii_slurp.o :
	$(CXX) $(CFALGS) $(SRC)/r3bascii_slurp.cc -fPIC -c

#-------------------------------------------------------------------------------------
#oct files

r3bascii_slurp_compressed_events : $(OBJECTS)
	$(C_OCT) $(OCTFLAGS) $(OBJECTS) $(SRC)/r3bascii_slurp_compressed_events.cc -o $(OCT)/r3bascii_slurp_compressed_events.oct
	rm r3bascii_slurp_compressed_events.o

r3bascii_slurp_events : $(OBJECTS)
	$(C_OCT) $(OCTFLAGS) $(OBJECTS) $(SRC)/r3bascii_slurp_events.cc -o $(OCT)/r3bascii_slurp_events.oct
	rm r3bascii_slurp_events.o


#-------------------------------------------------------------------------------------
#phony targets

.PHONY: all
all : $(OBJECTS) $(OCTS)
	rm $(OBJECTS)

.PHONY : clean
clean :
	rm -f $(OCTFILES) $(OBJECT)


