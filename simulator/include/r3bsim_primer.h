//a slot in function that will allow the new configuration interface to be used
//by FairRunSim.

#ifndef R3BSIM_PRIMER__H
#define R3BSIM_PRIMER__H

#include <functional>

#include "TVirtualMC.h" //for the globbbal hobbject gMC.
#include "TGeant3.h"
#include "TGeant3TGeo.h"
#include "R3BMCStack.h"

#include "r3b_ascii_parsecfg.h"

class r3bsim_primer : public std::function<void()> {
public:
	r3bsim_primer() {};
	r3bsim_primer( const r3bsim_primer &given );
	r3bsim_primer( const r3bsim_g3setup &gv_g3,
	               const r3bsim_cuts &gv_cuts,
	               const r3bsim_opts &gv_opts );
	~r3bsim_primer() {};
	
	r3bsim_primer &operator=( const r3bsim_primer &given );
	void operator()();
private:
	r3bsim_g3setup _g3;
	r3bsim_cuts _cuts;
	r3bsim_opts _opts;
};

#endif
