#define CATCH_CONFIG_MAIN
#include "catch.hh"
#include "store.hh"
#include "abi.hh"

using namespace bad;
using namespace std;

TEST_CASE( "wat", "[store]" ) {
  dl open;
  store<int,seq<4,5>> s;
  s = { 40,30,20,10}; // set rows
  s[1][1] = 2; // set individual elements
  s[2] = 3; // set an entire row
  s[3] = {1,2,3}; // set part of a row, element by element
  REQUIRE(sizeof(s) == 4*5*sizeof(int));
  REQUIRE(s[1][1] == 2);
  REQUIRE(s[3][4] == 10);
  REQUIRE(static_cast<void*>(&s) == static_cast<void*>(&s[0]));
  REQUIRE(static_cast<void*>(&s) == static_cast<void*>(&s[0][0]));
  REQUIRE(reinterpret_cast<byte*>(&s[1]) - reinterpret_cast<byte*>(&s[0]) == 5*sizeof(int));
  REQUIRE(reinterpret_cast<byte*>(&s[1][1]) - reinterpret_cast<byte*>(&s[1]) == sizeof(int));
  REQUIRE(type(s[1][1]) == "int");
  cout << s << endl;


//  s[1][2] = 3;
  //REQUIRE (s[1][2] == 3);
}