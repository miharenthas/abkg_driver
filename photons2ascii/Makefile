#Makefile to build this tiny library AND program

progs_home = $(PWD)
src_d = $(progs_home)/src
include_d = $(progs_home)/include
bin_d = $(progs_home)/bin
lib_d = $(progs_home)/lib

cxx_flags = -lgsl -lm -lcblas -ggdb -pthread -fopenmp -I $(include_d) -Wno-narrowing

objects = p2a_angtools p2a_gen_angdist p2a_gen_momenta p2a_dboost p2a_make_evt
libs = libp2a.a

ofiles = $(bin_d)/p2a_angtools.o $(bin_d)/p2a_gen_angdist.o $(bin_d)/p2a_gen_momenta.o $(bin_d)/p2a_dboost.o 4(bin_d)/p2a_make_evt.o $(lib_d)/libp2a.a

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
p2a_angtools :
	$(CXX) $(src_d)/p2a_angtools.cc $(cxx_flags) -fPIC -c -o $(bin_d)/p2a_angtools.o

p2a_gen_angdist :
	$(CXX) $(src_d)/p2a_gen_angdist.cc $(cxx_flags) -fPIC -c -o $(bin_d)/p2a_gen_angdist.o
	
p2a_gen_momenta :
	$(CXX) $(src_d)/p2a_gen_momenta.cc $(cxx_flags) -fPIC -c -o $(bin_d)/p2a_gen_momenta.o

p2a_dboost :
	$(CXX) $(src_d)/p2a_dboost.cc $(cxx_flags) -fPIC -c -o $(bin_d)/p2a_dboost.o

p2a_make_evt :
	$(CXX) $(src_d)/p2a_make_evt.cc $(cxx_flags) -fPIC -c -o $(bin_d)/p2a_make_evt.o

