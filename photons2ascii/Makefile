#Makefile to build this tiny library AND program

progs_home = $(PWD)
src_d = $(progs_home)/src
include_d = $(progs_home)/include
bin_d = $(progs_home)/bin
lib_d = $(progs_home)/lib

cxx_flags = -lgsl -lm -lcblas -ggdb -pthread -fopenmp -I $(include_d) -Wno-narrowing

objects = p2a_gen_angdist p2a_gen_momenta
ofiles = $(bin_d)/p2a_gen_angdist.o $(bin_d)/p2a_gen_momenta.o

CXX = g++ #fix it, we don't want clang

#-------------------------------------------------------------------------------------
#phony
.PHONY: all
all: $(objects)

.PHONY: clean
clean:
	rm -rf $(ofiles)

#-------------------------------------------------------------------------------------
#objects
p2a_gen_angdist :
	$(CXX) $(src_d)/p2a_gen_angdist.cc $(cxx_flags) -c -o $(bin_d)/p2a_gen_angdist.o
	
p2a_gen_momenta :
	$(CXX) $(src_d)/p2a_gen_momenta.cc $(cxx_flags) -c -o $(bin_d)/p2a_gen_momenta.o



