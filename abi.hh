#pragma once
#include <string>
#include <typeinfo>

namespace bad {
  namespace detail {
    std::string demangle(const char * name);
  }

  template <class T> std::string type(const T & t) {
    return detail::demangle(typeid(t).name());
  }
}
