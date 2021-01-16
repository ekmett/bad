#include "ad.h"
#include <iostream>
#include <tuple>

#include <dlfcn.h>
using namespace autodiff;

// i have one off by one error and its infuriating.
int main (int argc, char ** argv) {
  void *handle = dlopen(nullptr, 0);
  auto result = diff([](auto x) { return x*x+x; }, 10);
  std::cout << "It lives: " << std::get<0>(result) << ", " << std::get<1>(result) << std::endl;
  dlclose(handle);
  return 0;
}
