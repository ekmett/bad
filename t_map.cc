#define CATCH_CONFIG_MAIN
#include "catch.hh"
#include "map.hh"
#include <iostream>
#include <string>

using namespace std;

#define M(x) #x ## s
TEST_CASE("map works","[str]") {
  std::string strings[] = { BAD_MAP_LIST(M,hello,world) }; // testing empty strings
  REQUIRE(sizeof(strings) / sizeof(string) == 2);
  REQUIRE(strings[1][1] == 'o');
  std::string empty[] = { BAD_MAP_LIST(M,,hello,,world,) }; // testing empty strings
  REQUIRE(sizeof(empty) / sizeof(string) == 5);
}
