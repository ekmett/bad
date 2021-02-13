#include "catch.hh"
#include "bad/links.hh"
#include <iostream>
#include <string>

using namespace bad;
using namespace std;

TEST_CASE( "link_cut works", "[links]" ) {
  auto x = link_cut("x"s);
  auto y = link_cut("y"s);
  REQUIRE(x != y);
  REQUIRE(!x.connected(y));
  x.link(y);
  REQUIRE(x.cost() == "yx"s);
  REQUIRE(x != y);
  REQUIRE(x.connected(y));
  auto z = x;
  REQUIRE(z == x);
  REQUIRE(z.cost() == "yx"s);
  x.cut();
  REQUIRE(x.cost() == "x"s);
  REQUIRE(!x.connected(y));
}

