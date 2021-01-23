#pragma once
#include <string>
#include <typeinfo>
#include "attributes.hh"

namespace bad {
  namespace types {
    BAD(hd)
    std::string demangle(char const * name);

    namespace common {
      template <class T>
      BAD(hd) std::string type(T const & t) {
        return demangle(typeid(t).name());
      }
    }
    namespace exports {
      using namespace common;
    }
  }
  using namespace bad::types::common;
}

