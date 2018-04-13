#This makefile compiles the R3BRoot extensions.
#NOTE: for compilation to succeed, you need to apply the patch
#      because enums can't be trivially extended in C++
export SHELL = /bin/bash
export r3bext_home = $(PWD)
export r3bext_bin = $(r3bext_home)/bin
export r3bext_lib = $(r3bext_home)/lib

#libraries and includes
ROOT_LIBS = `root-config --libs`
FAIR_LIBS =  -lFairTools -lParBase -lBase -lGeom
R3B_LIBS = -lR3Bbase -lR3BData
ROOT_LIBS += $(FAIR_LIBS)
ROOT_LIBS += $(R3B_LIBS)

ROOT_FLAGS = `root-config --cflags`
ROOT_FLAGS += -L $(SIMPATH)/lib -L $(FAIRROOTPATH)/lib -I $(FAIRROOTPATH)/include

export ROOT_FLAGS
export ROOT_LIBS

#check the environment
ifeq ($(strip $(FAIRROOTPATH)),)
	$(error FairRoot is not available)
endif
ifeq ($(strip $(R3BROOTPATH)),)
	$(error R3BRoot is not available)
endif

#first thing first, check if the build directories exist
#if not, create them

.PHONY: all
all : dirz mvplanes passive

.PHONY: clean
clean :
	rm -f $(r3bext_bin)/passive/*
	rm -f $(r3bext_bin)/mvplanes/*
	rm -f $(r3bext_lib)/*.so

#check the filesystem
.PHONY: dirz
dirz :
	if ! [ -d $(r3bext_bin) ]; then \
		mkdir -p $(r3bext_bin) \
		mkdir -p $(r3bext_bin)/passive \
		mkdir -p $(r3bext_bin)/mvplanes; fi
	if ! [ -d $(r3bext_lib) ]; then mkdir $(r3bext_lib); fi

.PHONY: install
install :
	./install.sh

mvplanes : dirz
	make -eC mvplanes/

passive : dirz
	make -eC passive/
