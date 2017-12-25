#This makefile instructs the creation of the "gbkg" program.
PROGS_HOME = $(PWD)
SRC_D = $(PROGS_HOME)/src
INCLUDE_D = $(PROGS_HOME)/include
BIN_D = $(PROGS_HOME)/bin

ISGCC14CAPABLE = $(shell expr `gcc -dumpversion | cut -f1 -d.` \>= 5)

ROOT_LIBS = `root-config --libs`
ROOT_FLAGS = `root-config --cflags`
CXX_FLAGS = -Wno-varargs -I $(INCLUDE_D) -ggdb

ifeq "$(ISGCC14CAPABLE)" "1"
	CXX_FLAGS += -std=c++14
else
	CXX_FLAGS += -std=c++11
endif

OBJECTS = gen_bkg_from_histo
BINARIES = $(BIN_D)/gen_bkg_from_histo.o
PROGRAMS = hst2ascii

#-------------------------------------------------------------------------------------
#phony
.PHONY: all
all: $(OBJECTS) $(PROGRAMS)

.PHONY: clean
clean:
	rm -rf $(BINARIES) $(PROGRAMS)
	
#-------------------------------------------------------------------------------------
#objects
gen_bkg_from_histo :
	$(CXX) $(SRC_D)/gen_bkg_from_histo.cc $(ROOT_FLAGS) $(CXX_FLAGS) -c -o $(BIN_D)/gen_bkg_from_histo.o

#-------------------------------------------------------------------------------------
#programs
hst2ascii : gen_bkg_from_histo
	$(CXX) $(SRC_D)/gen_bkg.cpp $(BIN_D)/gen_bkg_from_histo.o $(ROOT_LIBS) $(ROOT_FLAGS) $(CXX_FLAGS) -o hst2ascii
	ln -sf hst2ascii gbkg
