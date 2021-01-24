#pragma once
#include <string>
#include <typeinfo>
#include "attributes.hh"

/// @file types.hh
/// @brief type names
///
/// @defgroup types types
/// @brief type names
/// @{

namespace bad {
  /// type names
  namespace types {
    /// re-exported by both \ref bad and \ref bad::types::api
    namespace common {}
    using namespace common;
    /// public components
    namespace api {
      using namespace common;
    }
    using namespace api;
  }
  using namespace types::common;
}

namespace bad::types {
  /// demangle a type name with the C++ ABI if available
  BAD(hd)
  std::string demangle(char const * name);
}

namespace bad::types::common {
  /// return the name of a given type given a reference
  template <class T>
  BAD(hd)
  std::string type(T const & t) noexcept {
    return demangle(typeid(t).name());
  }

  /// return the name of a given type without passing a reference
  template <class T>
  BAD(hd)
  std::string type_name() noexcept {
    return demangle(typeid(T).name());
  }

}

/// @}
