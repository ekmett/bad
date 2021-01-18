#define CATCH_CONFIG_MAIN
#include "catch.hh"
#include "shape.hh"

using namespace bad;
using namespace std;

TEST_CASE( "seq_head works", "[seq]" ) {
   REQUIRE( seq_head<seq<1,2,3>> == 1 );
   REQUIRE( seq_head<std::integer_sequence<std::size_t,4,2,1> > == 4 );
}
