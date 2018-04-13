#this makefile instructs the compilation of the passive bits

all_passives = R3BTargetAssemblyMedia.o R3BTargetAtmosphere.o R3BTarget.o R3BTargetShielding.o R3BTargetWheel.o R3BPassive_custom_dict.o libR3BPassive_custom.so

.PHONY: passive
passive : $(all_passives)

R3BPassive_custom_dict.o :
	rootcling $(r3bext_bin)/passive/R3BPassive_custom_dict.C -c R3B*.h PassiveCustomLinkDef.h
	g++ $(r3bext_bin)/passive/R3BPassive_custom_dict.C $(ROOT_FLAGS) -fPIC -c -o $(r3bext_bin)/passive/$@
R3BTargetAssemblyMedia.o :
	g++ R3BTargetAssemblyMedia.cxx -I passive $(ROOT_FLAGS) -fPIC -c -o $(r3bext_bin)/passive/R3BTargetAssemblyMedia.o
R3BTargetAtmosphere.o :
	g++ R3BTargetAtmosphere.cxx -I passive $(ROOT_FLAGS) -fPIC -c -o $(r3bext_bin)/passive/R3BTargetAtmosphere.o
R3BTarget.o :
	g++ R3BTarget.cxx -I passive $(ROOT_FLAGS) -fPIC -c -o $(r3bext_bin)/passive/R3BTarget.o
R3BTargetShielding.o :
	g++ R3BTargetShielding.cxx -I passive $(ROOT_FLAGS) -fPIC -c -o $(r3bext_bin)/passive/R3BTargetShielding.o
R3BTargetWheel.o :
	g++ R3BTargetWheel.cxx -I passive $(ROOT_FLAGS) -fPIC -c -o $(r3bext_bin)/passive/R3BTargetWheel.o

libR3BPassive_custom.so :
	g++ $(r3bext_bin)/passive/*.o -shared -o $(r3bext_lib)/libR3BPassive_custom.so
