#this makefile is for the bodglogger

all_boogies = r3b_ascii_blog.o r3b_ascii_bodgelogger.o bodgelogger_dict.o libbodgelogger.so

.PHONY: boogies
boogies: $(all_boogies)

bodgelogger_dict.o :
	rootcling $(r3bext_bin)/bodgelogger/bodgelogger_dict.C -c r3b_*.h BodgeloggerLinkDef.h
	g++ $(r3bext_bin)/bodgelogger/bodgelogger_dict.C $(ROOT_FLAGS) -fPIC -c -o $(r3bext_bin)/mvplanes/$@
r3b_ascii_blog.o :
	g++ r3b_ascii_blog.cc $(ROOT_FLAGS) -fPIC -c -o $(r3bext_bin)/bodgelogger/$@
r3b_ascii_bodgelogger.o :
	g++ r3b_ascii_bodgelogger.cc $(ROOT_FLAGS) -fPIC -c -o $(r3bext_bin)/bodgelogger/$@
libbodgelogger.so :
	g++ $(r3bext_bin)/bodgelogger/*.o $(ROOT_FLAGS) -shared -o $(r3bext_bin)/bodgelogger/$@
