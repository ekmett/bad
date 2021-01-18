#include "ad.hh"
#include <iostream>
#include <tuple>

using namespace autodiff;

// i have one off by one error and its infuriating.
int main (int argc, char ** argv) {
  dl open;
  auto result = diff([](auto x) { auto y = x*x; auto z = y*y; auto w = z*z; return w*w; }, 2);
  std::cout << "It lives: " << std::get<0>(result) << ", " << std::get<1>(result) << std::endl;
  return 0;
}
