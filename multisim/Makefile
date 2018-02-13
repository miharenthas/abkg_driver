#This very, very simple Makefile compiles evtmux
#actually, just saves a tiny bit of typing.

.PHONY : all
all : evtmux evtfunnel

evtmux :
	gcc -std=c99 evtmux.c -o evtmux

evtfunnel :
	gcc -std=c99 -lm evtfunnel.c -o evtfunnel

.PHONY : install
install : evtmux
	./install.sh

.PHONY : uninstall
uninstall :
	./install.sh -u
