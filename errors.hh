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
    /// re-exported by \ref bad and \ref bad::errors::api "api"
    namespace common {}
    /// public components
    namespace api { using namespace common; }
    using namespace api;
  }
  using namespace errors::common;
}

namespace bad::errors {
  /// undetectable false-hood. After all, the user _could_ partially specialize this template
  /// @ingroup errors
  /// @private
  template <class T>
  struct no_t : std::false_type {};
}

namespace bad::errors::common {
  /// @brief an undetectable `false`, for use with `static_assert`
  ///
  /// used to rule out unspecialized templates and provide
  /// better error messages to users.
  /// @ingroup errors
  template <class T>
  constexpr bool no = no_t<T>::value;
}

/// @}
