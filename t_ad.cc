#define CATCH_CONFIG_MAIN
#include "catch.hh"
#include "ad.hh"
#include <iostream>
#include <tuple>

using namespace bad;

TEST_CASE( "diff", "[ad]" ) {
  dl open;
  auto [x,y] = diff([](auto x) { auto y = x*x; auto z = y*y; auto w = z*z; return w*w; }, 2);
  REQUIRE(x == 65536);
  REQUIRE(y == 524288);
}

