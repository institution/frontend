#define CATCH_CONFIG_MAIN
#include "catch.hpp"


#include "sysinc.h"
#include "sys.h"


using namespace sys;

TEST_CASE( "sys", "" ) {
	Sys sys;
	sys.create_part({+1,0,0});
	sys.create_part({-1,0,0});
	
	auto fs = calc_forces(sys);
	REQUIRE(fs.at(0) == v3r(0,0,0));
	REQUIRE(fs.at(1) == v3r(0,0,0));
	
}

