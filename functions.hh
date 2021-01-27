#pragma once

#include "common.hh"
#include "attributes.hh"

/// \file
/// \brief for use with \ref bad::sequences::api::seq_zip "seq_zip" and the like
/// \author Edward Kmett

/// \defgroup functions_group functions
/// \brief functions

/// \namespace bad
/// \private
namespace bad {
  /// \namespace bad::functions
  /// \ref functions_group "functions" internals, import bad::functions::api
  /// \ingroup functions_group
  namespace functions {
    /// \namespace bad::functions::common
    /// \ingroup functions_group
    /// re-exported by \ref bad and bad::functions::api
    namespace common {}
    /// \namespace bad::functions::api
    /// \ingroup functions_group
    /// See \ref functions_group "functions" for a complete listing.
    namespace api { using namespace common; }
    using namespace api;
  }
  using namespace functions::common;
}

/// \addtogroup functions_group
/// \{
namespace bad::functions::common {
  /// inferrable `std::integral_constant`, used to encode lists as heterogenous lists
  /// \ingroup functions_group
  template <auto x>
  using ic = std::integral_constant<decltype(x), x>;
}

  /// \private
namespace bad::functions::api {

  /// \ingroup functions_group
  template <template <auto X, auto Y> class F, auto X>
  struct pap_auto {
    template <auto Y>
    using at = F<X,Y>;
  };

  /// \ingroup functions_group
  template <template <auto X, auto Y> class F>
  struct BAD(empty_bases) curry_auto {
    template <auto X>
    using at = pap_auto<F,X>; 
  };
  /// \ingroup functions_group
  template <auto X, auto Y>
  using eq = constant<X == Y>;

  /// \ingroup functions_group
  template <auto X, auto Y>
  using neq = constant<X != Y>;

  /// \ingroup functions_group
  template <auto X, auto Y>
  using lt = constant<(X < Y)>;

  /// \ingroup functions_group
  template <auto X, auto Y>
  using le = constant<(X <= Y)>;

  /// \ingroup functions_group
  template <auto X, auto Y>
  using ge = constant<(X >= Y)>;

  /// \ingroup functions_group
  template <auto X, auto Y>
  using gt = constant<(X > Y)>;

  /// \ingroup functions_group
  template <auto X, auto Y>
  using plus = constant<X + Y>;

  /// \ingroup functions_group
  template <auto X, auto Y>
  using minus = constant<X - Y>;

  /// \ingroup functions_group
  template <auto X, auto Y>
  using times = constant<X * Y>;

  /// \ingroup functions_group
  template <auto X, auto Y>
  using div = constant<X / Y>;

  /// \ingroup functions_group
  template <auto X, auto Y>
  using mod = constant<X % Y>;

  /// \ingroup functions_group
  template <auto X>
  using negate = constant<-X>;

  /// \ingroup functions_group
  template <auto X, auto Y>
  using bitwise_and = constant<X & Y>;

  /// \ingroup functions_group
  template <auto X, auto Y>
  using bitwise_or = constant<X | Y>;

  /// \ingroup functions_group
  template <auto X, auto Y>
  using bitwise_xor = constant<X ^ Y>;

  /// \ingroup functions_group
  template <auto X>
  using bitwise_complement = constant<~X>;

  /// \ingroup functions_group
  template <auto X, auto Y>
  using logical_and = constant<X && Y>;

  /// \ingroup functions_group
  template <auto X, auto Y>
  using logical_or = constant<X || Y>;

  /// \ingroup functions_group
  template <auto X>
  using logical_not = constant<!X>;

  /// \ingroup functions_group
  template <auto X, auto Y>
  using shift_left = constant<(X << Y)>;

  /// \ingroup functions_group
  template <auto X, auto Y>
  using shift_right = constant<(X >> Y)>;
}

/// \}
