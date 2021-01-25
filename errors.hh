#pragma once

#include <type_traits>

// bad_module(errors,user errors)


/// @file errors.hh
/// better user errors

/// @defgroup errors_group errors
/// better user errors
/// @{

namespace bad {
  /// @ref errors_group "errors" internals. import the bad::errors::api namespace
  namespace errors {
    /// also re-exported by \ref bad and \ref bad::errors::api "api"
    namespace common {}
    /// See @ref errors_group "errors" for a complete listing.
    /// (this listing will fail to show names supplied by \ref bad::errors::common "common")
    namespace api { using namespace common; }
    using namespace api;
  }
  using namespace errors::common;
}

namespace bad::errors {
  /// undetectable false-hood. After all, the user _could_ partially specialize this template
  /// @ingroup errors_group
  /// @private
  template <class T>
  struct no_t : std::false_type {};
}

namespace bad::errors::common {
  /// @brief an undetectable `false`, for use with `static_assert`
  ///
  /// used to rule out unspecialized templates and provide
  /// better error messages to users.
  /// @ingroup errors_group
  template <class T>
  constexpr bool no = no_t<T>::value;
}

/// @}
