#pragma once
#include <string>
#include <typeinfo>
#include "attrib.hh"

namespace bad {
  namespace detail {
    BAD(hd)
    std::string demangle(char const * name);
  }

  template <class T>
  BAD(hd) std::string type(T const & t) {
    return detail::demangle(typeid(t).name());
  }
}
