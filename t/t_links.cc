#include "catch.hh"
#include "bad/links.hh"
#include <iostream>

using namespace bad;
using namespace std;

TEST_CASE( "link_cut works", "[links]" ) {
  auto x = link_cut(1);
  auto y = link_cut(2);
  REQUIRE(x != y);
  REQUIRE(!x.connected(y));
  x.link(y);
  REQUIRE(x != y);
  REQUIRE(x.connected(y));
  auto z = x;
  REQUIRE(z == x);
}
