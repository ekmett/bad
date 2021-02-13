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

TEST_CASE( "lca works", "[links]" ) {
  auto x = link_cut("x"s), y = link_cut("y"s), z = link_cut("z"s), w = link_cut("w"s);
  //   w
  //   y
  //  x z

  y.link(w);
  x.link(y);
  z.link(y);
  REQUIRE(y == z.lca(x));
  // cout << "'" << y.cost() << "'"<< endl;
  // cout << "'" << y.lca(y).cost() << "'" << endl;
  REQUIRE(y == y.lca(y));
  REQUIRE(y == y.lca(y));
  REQUIRE(y == y.lca(x));
  REQUIRE(y == z.lca(y));
  REQUIRE(w == w.lca(y));
}
