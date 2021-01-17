#include "tape.h"
#include <iostream>
#include <array>
#include <tuple>

using namespace bad;
using namespace std;

struct simple : detail::static_propagator<5, simple, int> {
  simple() noexcept {

  }
  ~simple() noexcept override{
  }
  inline void prop(act_t, index_t &) const noexcept {}
  std::array<int,100> padding;
};

struct complx : detail::static_propagator<1, complx, int> {
  complx() noexcept {
  }
  ~complx() noexcept override{
  }
  inline void prop(act_t, index_t &) const noexcept {}
  std::array<int,1000> padding;
};

// i have one off by one error and its infuriating.
int main (int argc, char ** argv) {
  dl open;
  tape<int> t;
  for (int i=0;i<20;++i) {
    t.push<simple>();
    t.push<complx>();
  }
  for (auto & p : t)
    cout << p << endl;
  cout << t.activations << endl;

  //auto result = diff([](auto x) { auto y = x*x; auto z = y*y; auto w = z*z; return w*w; }, 2);
  //std::cout << "It lives: " << std::get<0>(result) << ", " << std::get<1>(result) << std::endl;
  return 0;
}
