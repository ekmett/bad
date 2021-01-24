#pragma once

#include <type_traits>

namespace bad {
  namespace errors {
    namespace common {}
    using namespace common;
    namespace api {
      using namespace common;
    }
    using namespace api;
  }
  using namespace errors::common;
}

namespace bad::errors {
  /// @private
  template <class T>
  struct no_t : std::false_type {};
}

namespace bad::errors::common {
  template <class T>
  constexpr bool no = no_t<T>::value;
}
