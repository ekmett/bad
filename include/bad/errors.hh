#ifndef BAD_ERRORS_HH
#define BAD_ERRORS_HH

#include <type_traits>

/// \file
/// better errors for users
///
/// \defgroup errors_group errors
/// better errors for users

/// \namespace bad
/// \private
namespace bad {
  /// \namespace bad::errors
  /// \ref errors_group "errors" internals, import bad::errors::api
  /// \ingroup errors_group
  namespace errors {
    /// \namespace bad::errors::common
    /// \ingroup errors_group
    /// re-exported by \ref bad and bad::errors::api
    namespace common {}
    /// \namespace bad::errors::api
    /// \ingroup errors_group
    /// See \ref errors_group "errors" for a complete listing.
    namespace api { using namespace common; }
    using namespace api;
  }
  using namespace errors::common;
}

/// \{
namespace bad::errors {
  /// undetectable `false`, traditional template form.
  /// After all, the user _could_ at least theoretically partially specialize this template
  ///
  /// \ingroup errors_group
  template <class T>
  struct no_ : std::false_type {};
}

namespace bad::errors::common {
  /// \brief an undetectable `false` as a variable template.
  ///
  /// For use with `static_assert` to allow me to rule out the unspecialized case when
  /// partial template specialization is required. This allows us to give the user
  /// informative error messaages, rather than just say the template isn't instantiated.
  ///
  /// \ingroup errors_group
  template <class T>
  constexpr bool no = no_<T>::value;
}
/// \}

#endif
