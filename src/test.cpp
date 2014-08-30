#define CATCH_CONFIG_MAIN
#include "catch.hpp"


#include "sysinc.h"
#include "sys.h"


using namespace sys;

TEST_CASE( "sys", "" ) {
	Sys sys;
	sys.create_part({+1,0,0});
	sys.create_part({-1,0,0});
	
	std::vector<v3r> fs;
	
	SECTION("no-link") {
		calc_forces(fs, sys);
		REQUIRE(fs.at(0) == v3r(0,0,0));
		REQUIRE(fs.at(1) == v3r(0,0,0));
	}
	
	SECTION("with-link") {
		sys.create_link(0,1, 1);

		calc_forces(fs, sys);
		REQUIRE(fs.at(0) == v3r(-0.5f,0,0));
		REQUIRE(fs.at(1) == v3r(+0.5f,0,0));
	}
	
}

