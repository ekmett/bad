#include "catch.hh"
#include "bad/disjoint_sets.hh"
#include <iostream>

using namespace bad::disjoint_sets::api;
using namespace std;

TEST_CASE( "ds<T> works", "[disjoint_sets]" ) {
  auto x = ds(1);
  auto y = ds(2);
  cout << "YOLO" << endl;
  REQUIRE(x.value() == 1);
  REQUIRE(x != y);
  REQUIRE(x.parent().p == x.p);
  merge(x,y,ds(34));
  REQUIRE(x.parent() == x);
  REQUIRE(x.value() == 37);
  REQUIRE(x == y);
}
