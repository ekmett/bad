#include <iostream>
#include <string>
#include <array>
#include <tuple>

#include "bad/sequences.hh"
#include "bad/tapes.hh"

#include "catch.hh"

using namespace std;
using namespace bad;

struct simp : static_record<5, simp, int> {
  inline void prop(act_t, size_t &) const noexcept {}
  std::array<int,5000> padding;
};

struct comp : static_record<1, comp, int> {
  inline void prop(act_t, size_t &) const noexcept {}
  std::array<int,1000> padding;
};

TEST_CASE("tape works","[tapes]") {
  tape<int> t;
  for (int i=0;i<15;++i) {
    t.push<simp>();
    t.push<comp>();
  }
  REQUIRE(t.activations == 90);
}
