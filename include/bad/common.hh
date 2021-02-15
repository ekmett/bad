#ifndef BAD_COMMON_HH
#define BAD_COMMON_HH

#include <iostream>
#include <algorithm> // std::max
#include <cstddef>   // std::size_t, std::ptrdiff_t
#include <utility>   // std::integer_sequence
#include <cmath>     // std::sin, etc.
#include <cinttypes> // abs(std::intmax_t)
#include <numeric>

#include "bad/attributes.hh"

/// \file
/// \brief common definitions, shared across all modules
///
/// \author Edward Kmett

/// \defgroup common_group common
/// \brief common definitions, shared across all modules

// explicitly allow contracting of floating-point expressions,
// that is to say, operations that reduce floating point error
#pragma STDC FP_CONTRACT

/// \namespace bad
/// \brief top level namespace
namespace bad {
  using std::size_t;
  using std::ptrdiff_t;

  /// inferrable `std::integral_constant`, used to encode lists as heterogenous lists
  /// \ingroup common_group
  template <auto x>
  using constant = std::integral_constant<decltype(x), x>;

  /// Sometimes we want expression templates to store values, sometimes we want to store references inside of our expression templates.
  ///
  /// This acts as a solution to the temporary capture problem whenever we need to build a combinator that builds temporaries
  /// safely. (It arises when working with einsum, or in the recursive cases for applying pull to a sum for instance.
  ///
  /// This looks at the type passed and if it is T&&, we store a value, otherwise we store a reference.
  ///
  /// In this way such combinators can pass in temporaries to the expression template and we should be able to capture them
  /// automatically, correctly and safely.
  /// \ingroup common_group
  template <class T>
  using sub_expr = std::conditional_t<
    std::is_rvalue_reference_v<T>,
    std::decay_t<T>,
    std::decay_t<T> const &
  >;

  // std numerics
  using std::abs;
  using std::acos;
  using std::acosh;
  using std::asin;
  using std::asinh;
  using std::atan2;
  using std::atan;
  using std::atanh;
  using std::cbrt;
  using std::cos;
  using std::cosh;
  using std::div;
  using std::exp2;
  using std::exp;
  using std::expm1;
  using std::fabs;
  using std::hypot;
  using std::log10;
  using std::log1p;
  using std::log2;
  using std::log;
  using std::lgamma;
  using std::pow;
  using std::sin;
  using std::sinh;
  using std::sqrt;
  using std::tan;
  using std::tanh;
  using std::tgamma;
  using std::erf;
  using std::erfc;
  using std::fmod;
  using std::remainder;
  using std::remquo;
  using std::fma;
  using std::fmax;
  using std::fmin;
  using std::fdim;
  using std::max;
  using std::min;
  using std::ceil;
  using std::floor;
  using std::trunc;
  using std::round;
  using std::nearbyint;
  using std::rint;
  using std::frexp;
  using std::ldexp;
  using std::modf;
  using std::scalbn;
  using std::ilogb;
  using std::logb;
  using std::nextafter;
  using std::nexttoward;
  using std::copysign;
  using std::fpclassify;
  using std::isfinite;
  using std::isinf;
  using std::isnan;
  using std::isnormal;
  using std::signbit;
  using std::isgreater;
  using std::isgreaterequal;
  using std::isless;
  using std::islessequal;
  using std::islessgreater;
  using std::isunordered;
  using std::float_t;
  using std::double_t;
}

#endif
