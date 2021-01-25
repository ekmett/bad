#pragma once
#include <string>
#include <typeinfo>
#include "attributes.hh"

/// @file types.hh
/// @brief type names
/// @author Edward Kmett
///
/// @defgroup types_group types
/// @brief type names
/// @{

namespace bad {
  /// The @ref types_group "types" module. This namespace holds internals. You should probably import bad::types::api instead.
  namespace types {
    /// re-exported by \ref bad and \ref bad::types::api "api"
    namespace common {}
    /// public components. See the @ref types_group "types" module for a complete listing
    namespace api { using namespace common; }
    using namespace api;
  }
  using namespace types::common;
}

namespace bad::types {
  /// demangle a type name with the C++ ABI if available
  /// @ingroup types_group
  /// @private
  BAD(hd)
  std::string demangle(char const * name);
}

namespace bad::types::common {
  /// return the name of a given type given a reference
  /// @ingroup types_group
  template <class T>
  BAD(hd)
  std::string type(T const & t) noexcept {
    return demangle(typeid(t).name());
  }

  /// return the name of a given type without passing a reference
  /// @ingroup types_group
  template <class T>
  BAD(hd)
  std::string type_name() noexcept {
    return demangle(typeid(T).name());
  }
}

/// @}
