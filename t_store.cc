#define CATCH_CONFIG_MAIN
#include "catch.hh"
#include "store.hh"
#include "abi.hh"

using namespace bad;
using namespace std;

TEST_CASE( "store works", "[store]" ) {
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

  store<int,seq<4,5>> t(2);
  cout << t << endl;

  // column major
  store<int,seq<4,5>,seq<1,4>> r = s - t;
  cout << r << endl;

  r.pull<1>(1) = 200;
  cout << r << endl;

  store<int,seq<4,5>> x;
  x.pull<1>() = {1,2,3,4,5};
  x.pull<1>(3)[2] = 100;
  REQUIRE(x[2][3] == 100);
  cout << x << endl;
}
