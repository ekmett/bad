#include "abi.hh"

#ifdef __GNUG__
#include <cstdlib>
#include <memory>
#include <cxxabi.h>

std::string bad::detail::demangle(char const * name) {
  int status = -4;
  std::unique_ptr<char, void(*)(void*)> res {
    abi::__cxa_demangle(name, NULL, NULL, &status),
    std::free
  };
  return (status==0) ? res.get() : name ;
}

#else

std::string bad::detail::demangle(char const * name) {
  return name;
}

#endif
