#This makefile instructs the creation of the "sbkg" program.
PROGS_HOME = $(PWD)
SRC_D = $(PROGS_HOME)/src
INCLUDE_D = $(PROGS_HOME)/include
BIN_D = $(PROGS_HOME)/bin

ROOT_LIBS = `root-config --libs`
GEANT_LIBS = -lgeant321 -lPythia6 -lVMC -lEG -lgfortran
FAIR_LIBS =  -lFairTools -lParBase -lBase
R3B_LIBS = -lELILuMon -lField -lR3Bbase -lR3BData -lR3BCalifa -lR3BDch -lR3BdTof -lR3BEvtVis -lR3BFi4 -lR3BGen -lR3BGfi -lR3BLand -lR3BLos -lR3BMfi -lR3BmTof -lR3BNeuland -lR3BPassive -lR3BPlist -lR3BPsp -lR3BSTaRTra -lR3BTCal -lR3BTof -lR3BTra -lR3BXBall -ltimestitcher -lR3BMVPlanes -lbodgelogger
ROOT_LIBS += $(FAIR_LIBS)
ROOT_LIBS += $(R3B_LIBS)
ROOT_LIBS += $(GEANT_LIBS)

ROOT_FLAGS = `root-config --cflags`
ROOT_FLAGS += -L $(SIMPATH)/lib -L $(FAIRROOTPATH)/lib -I $(SIMPATH)/include -I $(SIMPATH)/include/TGeant3 -I $(FAIRROOTPATH)/include
CXX_FLAGS = -Wno-varargs -std=c++11 -I $(INCLUDE_D) -pthread -ggdb

OBJECTS = r3b_ascii_gen r3bsim_driver r3b_ascii_event_structs r3b_ascii_paged_stack
BINARIES = $(BIN_D)/r3b_ascii_gen.o $(BIN_D)/r3bsim_driver.o $(BIN_D)/r3b_ascii_event_structs.o $(BIN_D)/r3b_ascii_paged_stack.o
PROGRAMS = sbkg

CXX = g++

#-------------------------------------------------------------------------------------
#phony
.PHONY: all
all: $(OBJECTS) $(PROGRAMS)

.PHONY: clean
clean:
	rm -rf $(BINARIES) $(PROGRAMS)

#-------------------------------------------------------------------------------------
#objects

r3b_ascii_gen :
	$(CXX) $(SRC_D)/r3b_ascii_gen.cc $(ROOT_FLAGS) $(CXX_FLAGS) -c -o $(BIN_D)/r3b_ascii_gen.o

r3bsim_driver :
	$(CXX) $(SRC_D)/r3bsim_driver.cc $(ROOT_FLAGS) $(CXX_FLAGS) -c -o $(BIN_D)/r3bsim_driver.o

r3b_ascii_event_structs :
	$(CXX) $(SRC_D)/r3b_ascii_event_structs.cc $(CXX_FLAGS) -c -o $(BIN_D)/r3b_ascii_event_structs.o

r3b_ascii_paged_stack :
	$(CXX) $(SRC_D)/r3b_ascii_paged_stack.cc $(CXX_FLAGS) -c -o $(BIN_D)/r3b_ascii_paged_stack.o

#-------------------------------------------------------------------------------------
#programs

sbkg : r3b_ascii_gen r3bsim_driver r3b_ascii_event_structs r3b_ascii_paged_stack
	$(CXX) $(SRC_D)/r3bsim_sim_ascii.cpp $(BIN_D)/r3b_ascii_gen.o $(BIN_D)/r3bsim_driver.o $(BIN_D)/r3b_ascii_event_structs.o $(BIN_D)/r3b_ascii_paged_stack.o $(ROOT_LIBS) $(ROOT_FLAGS) $(CXX_FLAGS) -o sbkg
