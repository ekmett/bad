#include <iostream>
#include <string>
#include <array>
#include <tuple>

#include "sequences.hh"
#include "tapes.hh"

#define CATCH_CONFIG_MAIN
#include "catch.hh"

using namespace std;
using namespace bad::tapes;

struct simp : static_propagator<5, simp, int> {
  inline void prop(act_t, size_t &) const noexcept {}
  std::array<int,5000> padding;
};

struct comp : static_propagator<1, comp, int> {
  inline void prop(act_t, size_t &) const noexcept {}
  std::array<int,1000> padding;
};

TEST_CASE("tape works","[str]") {
  tape<int> t;
  for (int i=0;i<15;++i) {
    t.push<simp>();
    t.push<comp>();
  }
  REQUIRE(t.activations == 90);
}
