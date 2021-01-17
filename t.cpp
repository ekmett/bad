#include "ad.h"
#include <iostream>
#include <tuple>

#include <dlfcn.h>
using namespace autodiff;

struct dl {
  dl(const char * filename = nullptr) : handle(dlopen(filename,0)) {}
  ~dl() { dlclose(handle); }
  void * handle;
};

// i have one off by one error and its infuriating.
int main (int argc, char ** argv) {
  dl open;
  auto result = diff([](auto x) { auto y = x*x; auto z = y*y; auto w = z*z; return w*w; }, 2);
  std::cout << "It lives: " << std::get<0>(result) << ", " << std::get<1>(result) << std::endl;
  return 0;
}
