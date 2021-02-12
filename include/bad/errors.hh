#ifndef BAD_ERRORS_HH
#define BAD_ERRORS_HH

#include <type_traits>

/// \file
/// better errors for users
///
/// \defgroup errors_group errors
/// better errors for users

namespace bad {
  namespace detail {
    /// undetectable `false`, traditional template form.
    /// After all, the user _could_ at least theoretically partially specialize this template
    ///
    /// \ingroup errors_group
    /// \meta
    template <class T>
    struct no_ : std::false_type {};
  }

  /// \brief an undetectable `false` as a variable template.
  ///
  /// For use with `static_assert` to allow me to rule out the unspecialized case when
  /// partial template specialization is required. This allows us to give the user
  /// informative error messaages, rather than just say the template isn't instantiated.
  ///
  /// \ingroup errors_group
  template <class T>
  constexpr bool no = detail::no_<T>::value;
}

#endif
