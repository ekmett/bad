#define CATCH_CONFIG_MAIN
#include "catch.hh"
#include "shape.hh"

using namespace bad;
using namespace std;

TEST_CASE( "seq_head works", "[shape]" ) {
   REQUIRE( seq_head<seq<1,2,3>> == 1 );
   REQUIRE( seq_head<std::integer_sequence<std::size_t,4,2,1> > == 4 );
}

TEST_CASE("stride works","[shape]") {
  REQUIRE( stride<0,int,7,5,3> == 15);
  REQUIRE( stride<1,int,7,5,3> == 3);
  REQUIRE( stride<2,int,7,5,3> == 1);
}

TEST_CASE("prod works","[shape]") {
  REQUIRE( prod<int,4,2,3> == 24 );
}

TEST_CASE("nth works","[shape]") {
  REQUIRE(nth<4,int,0,10,20,30,40,50,60> == 40);
}

TEST_CASE("seq_nth works","[shape]") {
   REQUIRE(seq_nth<4,seq<0,11,21,31,41,51>> == 41);
}
TEST_CASE("seq_length works","[shape]") {
   REQUIRE(seq_length<seq<3,2,1>> == 3);
}