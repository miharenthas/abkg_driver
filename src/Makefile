####   AXP
# COMPILE => f77 -DCERNLIB_TYPE -g2 -c -O0 -check bounds -extend_source 
# LINK => f77 -g0 -O2

#
# linux with gfortran and the ubuntu cernlib packages: libpacklib1-dev cernlib-base-dev
# the cernlib command should be available
# 

FC=gfortran
FFLAGS=-W -Wall -g2 -O0 -fbounds-check -ffixed-line-length-none
LDFLAGS=-g2 -O2
LIBS=$(shell cernlib packlib)

all: abkg 

abkg: abkg.o
	$(FC) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f abkg.o abkg


