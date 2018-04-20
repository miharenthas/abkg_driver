all_mvplanes = R3BRattlePlane.o R3BRPHit.o R3BStopperPlane.o R3BMVPlanes_dict.o libR3BMVPlanes.so

.PHONY: mvplanes
mvplanes : $(all_mvplanes)

R3BMVPlanes_dict.o :
	rootcling $(r3bext_bin)/mvplanes/R3BMVPlanes_dict.C -rml libR3BMVPlanes.so -rmf $(r3bext_bin)/mvplanes/libR3BMVPlanes.rootmap -c R3B*.h MVPLinkDef.h
	g++ $(r3bext_bin)/mvplanes/R3BMVPlanes_dict.C $(ROOT_FLAGS) -fPIC -c -o $(r3bext_bin)/mvplanes/$@
R3BRattlePlane.o :
	g++ R3BRattlePlane.cxx $(ROOT_FLAGS) -fPIC -c -o $(r3bext_bin)/mvplanes/R3BRattlePlane.o
R3BRPHit.o :
	g++ R3BRPHit.cxx $(ROOT_FLAGS) -fPIC -c -o $(r3bext_bin)/mvplanes/R3BRPHit.o
R3BStopperPlane.o :
	g++ R3BStopperPlane.cxx $(ROOT_FLAGS) -fPIC -c -o $(r3bext_bin)/mvplanes/R3BStopperPlane.o
libR3BMVPlanes.so :
	g++ $(r3bext_bin)/mvplanes/*.o -shared -o $(r3bext_lib)/libR3BMVPlanes.so
