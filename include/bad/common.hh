#ifndef BAD_COMMON_HH
#define BAD_COMMON_HH

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
/// other modules may extend this functionality, and add more things here,
/// but this offers a place for small things that have no good home, or
/// which rarely require the full power of their 'proper' home.
//
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

  /// shorthand for `std::integer_sequence`
  /// \ingroup common_group
  template <class T, T... is>
  using iseq = std::integer_sequence<T, is...>;

  /// \ref iseq with type inference, so long as there is at least one argument
  /// \ingroup common_group
  template <auto... xs>
  using aseq = iseq<std::common_type_t<decltype(xs)...>, xs...>;


  /// A \ref iseq "sequence" of `size_t` "sizes". used to store dimensions. a.k.a. `std::index_sequence`
  /// \ingroup common_group
  template <size_t... is>
  using seq = iseq<size_t, is...>;

  /// A \ref iseq "sequence" of `ptrdiff_t` "signed distances". Used to store strides.
  /// \ingroup common_group
  template <ptrdiff_t... is>
  using sseq = iseq<ptrdiff_t, is...>;

  /// A compile-time string as a type of \ref iseq "sequence". `char` is an integral type in C++.
  /// \ingroup common_group
  template <char...cs>
  using str = iseq<char, cs...>;

#ifdef __clang__
#ifndef ICC // icpc is a lying liar that lies
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-string-literal-operator-template"
#endif
#elif defined __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-string-literal-operator-template"
#endif

  /// allows `decltype("foo"_str)` to return the type `str<'f','o','o'>`
  /// \ingroup common_group
  template <class T, T...cs>
  BAD(hd,const)
  str<cs...> operator""_str() noexcept {
    return {};
  }

#ifdef __clang__
#ifndef ICC
#pragma clang diagnostic pop
#endif
#elif defined __GNUC__
#pragma GCC diagnostic pop
#endif

  /// if-then-else
  /// \ingroup common_group
  template <bool B, class T, class E>
  using ite = std::conditional_t<B,T,E>;

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
