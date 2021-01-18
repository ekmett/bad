#define CATCH_CONFIG_MAIN
#include "catch.hh"
#include "abi.hh"
#include "einsum.hh"

using namespace std;
using namespace bad;

TEST_CASE( "str", "[einsum]" ) {
  auto & x = "wat"_sym;
  auto & y = "wat"_sym;
  REQUIRE(&x == &y);
  REQUIRE(type(x) == "bad::sym<(char)119, (char)97, (char)116>");
  REQUIRE("wat"s == std::string(x));

  REQUIRE(bad::find<int,1,3,4,2,1,4> == 3);
  REQUIRE(sym_find<'a',sym<'w','a','t'>> == 1);
  REQUIRE(sym_find<'z',sym<'w','a','t'>> == 3);

  constexpr std::string cs = "yolo"s;

  //we're able to make string literal objects, and check them for equality. not we just need to write a parser.
}
