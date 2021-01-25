#pragma once

#include <type_traits>

/// @file errors.hh
/// better user errors

/// @defgroup errors errors
/// better user errors
/// @{

namespace bad {
  /// better user errors
  namespace errors {
    /// re-exported by \ref bad and \ref bad::errors::api
    namespace common {}
    /// public components
    namespace api { using namespace common; }
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
  /// an undetectable `false`, for use with `static_assert`
  /// used to rule out unspecialized templates and provide
  /// better error messages to users.
  template <class T>
  constexpr bool no = no_t<T>::value;
}

/// @}
