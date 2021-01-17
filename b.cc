#include "tape.h"
#include "shape.h"
#include "bad.h"
#include <iostream>
#include <array>
#include <tuple>

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

  cout << stride<0,int,7,5,3> << endl; // = 15
  cout << stride<1,int,7,5,3> << endl; // = 3
  cout << stride<2,int,7,5,3> << endl; // = 1

  cout << stride<0,int,10,4,100> << endl;
  cout << prod<int,1,2> << endl;
  cout << nth<4,int,0,10,20,30,40,50,60>() << endl;

  tape<int> t;
  for (int i=0;i<15;++i) {
    t.push<simple>();
  }
  for (auto & p : t)
    cout << p << endl;
  cout << t.activations << endl;

  //auto result = diff([](auto x) { auto y = x*x; auto z = y*y; auto w = z*z; return w*w; }, 2);
  //std::cout << "It lives: " << std::get<0>(result) << ", " << std::get<1>(result) << std::endl;
  return 0;
}
