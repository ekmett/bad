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
      BAD(hd)
      std::string type(T const & t) {
        return demangle(typeid(t).name());
      }
    }
    using namespace common;
    namespace api {
      using namespace common;
    }
    using namespace api;
  }
  using namespace bad::types::common;
}

