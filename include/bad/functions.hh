#ifndef BAD_FUNCTIONS_HH
#define BAD_FUNCTIONS_HH

#include "attributes.hh"
#include "common.hh"

/// \file
/// \brief for use with \ref bad::seq_zip "seq_zip" and the like
/// \author Edward Kmett

/// \defgroup functions_group functions
/// \brief templated functions

/// \addtogroup functions_group
/// \{
namespace bad {
  /// \defgroup unary_functions_group unary
  /// \brief templated unary operators
  /// \ingroup functions_group
  /// \{

  /// \ingroup unary_functions_group
  template <auto x>
  using logical_not = constant<!x>;

  /// \ingroup unary_functions_group
  template <auto x>
  using negate = constant<-x>;

  /// \ingroup unary_functions_group
  template <auto x>
  using complement = constant<~x>;

  /// \}

  /// \defgroup binary_functions_group binary
  /// \brief templated binary operators
  /// \ingroup functions_group
  /// \{

  /// \ingroup binary_functions_group
  template <auto x, auto y>
  using eq = constant<x == y>;

  /// \ingroup binary_functions_group
  template <auto x, auto y>
  using neq = constant<x != y>;

  /// \ingroup binary_functions_group
  template <auto x, auto y>
  using lt = constant<(x < y)>;

  /// \ingroup binary_functions_group
  template <auto x, auto y>
  using le = constant<(x <= y)>;

  /// \ingroup binary_functions_group
  template <auto x, auto y>
  using ge = constant<(x >= y)>;

  /// \ingroup binary_functions_group
  template <auto x, auto y>
  using gt = constant<(x > y)>;

  /// \ingroup binary_functions_group
  template <auto x, auto y>
  using plus = constant<x+y>;

  /// \ingroup binary_functions_group
  template <auto x, auto y>
  using minus = constant<x-y>;

  /// \ingroup binary_functions_group
  template <auto x, auto y>
  using times = constant<x * y>;

  /// \ingroup binary_functions_group
  template <auto x, auto y>
  using div = constant<x / y>;

  /// \ingroup binary_functions_group
  template <auto x, auto y>
  using mod = constant<x % y>;

  /// \ingroup binary_functions_group
  template <auto x, auto y>
  using bitwise_and = constant<x & y>;

  /// \ingroup binary_functions_group
  template <auto x, auto y>
  using bitwise_or = constant<x | y>;

  /// \ingroup binary_functions_group
  template <auto x, auto y>
  using bitwise_xor = constant<x ^ y>;

  /// \ingroup binary_functions_group
  template <auto x, auto y>
  using logical_and = constant<x && y>;

  /// \ingroup binary_functions_group
  template <auto x, auto y>
  using logical_or = constant<x || y>;

  /// \ingroup binary_functions_group
  template <auto x, auto y>
  using shift_left = constant<(x << y)>;

  /// \ingroup binary_functions_group
  template <auto x, auto y>
  using shift_right = constant<(x >> y)>;

  /// \}

  /// \defgroup fold_functions_group folds
  /// \brief templated folds
  /// \ingroup functions_group
  /// \{

  /// \ingroup fold_functions_group
  template <auto... xs>
  using fold_plus = constant<(xs + ...)>;

  /// \ingroup fold_functions_group
  template <auto... xs>
  using fold_minus = constant<(xs - ...)>;

  /// \ingroup fold_functions_group
  template <auto... xs>
  using fold_times = constant<(xs * ...)>;

  /// \ingroup fold_functions_group
  template <auto... xs>
  using fold_div = constant<(xs / ...)>;

  /// \ingroup fold_functions_group
  template <auto... xs>
  using fold_mod = constant<(xs % ...)>;

  /// \ingroup fold_functions_group
  template <auto... xs>
  using fold_bitwise_and = constant<(xs & ...)>;

  /// \ingroup fold_functions_group
  template <auto... xs>
  using fold_bitwise_or = constant<(xs | ...)>;

  /// \ingroup fold_functions_group
  template <auto... xs>
  using fold_bitwise_xor = constant<(xs ^ ...)>;

  /// \ingroup fold_functions_group
  template <auto... xs>
  using fold_logical_and = constant<(xs && ...)>;

  /// \ingroup fold_functions_group
  template <auto... xs>
  using fold_logical_or = constant<(xs || ...)>;

  /// \ingroup fold_functions_group
  template <auto... xs>
  using fold_shift_left = constant<(xs << ...)>;

  /// \ingroup fold_functions_group
  template <auto... xs>
  using fold_shift_right = constant<(xs >> ...)>;
  /// \}

  /// \ingroup functions_group
  template <template <auto...> class F>
  struct v2u {
    template <auto X>
    using at = F<X>;
  };

  /// \ingroup functions_group
  template <template <auto> class F>
  struct u2v { // forbids prevents partial template specialization
    template <auto... xs>
    using at = F<xs...>;
  };

  /// \ingroup functions_group
  template <template <auto...> class F>
  struct v2b {
    template <auto X, auto Y>
    using at = F<X,Y>;
  };

  /// \ingroup functions_group
  template <template <auto,auto> class F>
  struct b2v {
    template <auto... xs>
    using at = F<xs...>;
  };

  /// \ingroup functions_group
  template <template <auto...> class F>
  struct v2v {
    template <auto... xs>
    using at = F<xs...>;
  };

  /// partial application of a variadic template head first
  /// \ingroup functions_group
  template <template <auto...> class F, auto... xs>
  struct BAD(empty_bases) pap {
    template <auto... ys>
    using at = F<xs...,ys...>;
  };

  /// partial application of a variadic template tail first
  /// \ingroup functions_group
  template <template <auto...> class F, auto... ys>
  struct BAD(empty_bases) pap_right {
    template <auto... xs>
    using at = F<xs...,ys...>;
  };

  template <template <auto,auto> class F, auto x>
  struct papb {
    template <auto y>
    using at = F<x,y>;
  };

  template <template <auto,auto> class F, auto y>
  struct flipb {
    template <auto x>
    using at = F<y,x>;
  };
}

/// \}

#endif
