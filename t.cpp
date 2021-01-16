#include "ad.h"
#include <iostream>
#include <tuple>

#include <dlfcn.h>
using namespace autodiff;

int main (int argc, char ** argv) {
  void *handle = dlopen(nullptr, 0);
  auto result = diff([](auto x) { return 5*x + x*3; }, 10);
  std::cout << "It lives: " << std::get<0>(result) << ", " << std::get<1>(result) << std::endl;
  dlclose(handle);
  return 0;
}
