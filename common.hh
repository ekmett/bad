#pragma once

#include <algorithm> // std::max
#include <cstddef>   // std::size_t, std::ptrdiff_t
#include <utility>   // std::integer_sequence

#include "attributes.hh"

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
/// \{

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
}

/// \}
