#define CATCH_CONFIG_MAIN
#include "catch.hh"
#include "shape.hh"

using namespace bad;
using namespace std;

TEST_CASE( "seq_head works", "[seq]" ) {
   REQUIRE( seq_head<seq<1,2,3>> == 1 );
   REQUIRE( seq_head<std::integer_sequence<std::size_t,4,2,1> > == 4 );
}

TEST_CASE("stride works","[seq]") {
  REQUIRE( stride<0,int,7,5,3> == 15);
  REQUIRE( stride<1,int,7,5,3> == 3);
  REQUIRE( stride<2,int,7,5,3> == 1);
}

TEST_CASE("prod works","[seq]") {
  REQUIRE( prod<int,4,2,3> == 24 );
}

TEST_CASE("nth works","[seq]") {
  REQUIRE(nth<4,int,0,10,20,30,40,50,60> == 40);
}
