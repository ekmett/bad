#include <iostream>
#include <array>
#include <tuple>
#include "seq.hh"
#include "tape.hh"

using namespace bad;
using namespace std;

struct simple : detail::static_propagator<5, simple, int> {
  inline void prop(act_t, index_t &) const noexcept {}
  std::array<int,5000> padding;
};

struct complx : detail::static_propagator<1, complx, int> {
  inline void prop(act_t, index_t &) const noexcept {}
  std::array<int,1000> padding;
};

// i have one off by one error and its infuriating.
int main (int argc, char ** argv) {
  dl open;

  tape<int> t;
  for (int i=0;i<15;++i) {
    t.push<simple>();
  }
  for (auto & p : t)
    cout << p << endl;
  cout << t.activations << endl;

  return 0;
}
