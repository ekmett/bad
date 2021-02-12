#include "catch.hh"
#include "bad/disjoint.hh"
#include <iostream>

using namespace bad;
using namespace std;

TEST_CASE( "ds<T> works", "[disjoint]" ) {
  auto x = disjoint(1);
  auto y = disjoint(2);
  cout << "YOLO" << endl;
  REQUIRE(x.value() == 1);
  REQUIRE(x != y);
  merge(x,y,disjoint(34));
  REQUIRE(x.value() == 37);
  REQUIRE(x == y);
}
