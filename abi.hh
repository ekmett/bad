#pragma once
#include <string>
#include <typeinfo>
#include "attrib.hh"

namespace bad {
  namespace detail {
    std::string demangle(const char * name);
  }

  template <class T> BAD_HD std::string type(const T & t) {
    return detail::demangle(typeid(t).name());
  }
}
