#pragma once
#include <array>

// a basic direct multi-dimensional array type in row-major order.
// other types probably should model this.
// we get bidirectional iterators at the top level that can be chased downward.

namespace bad {
  namespace detail {
    template <typename T, size_t ... Shape>
    struct holor_ {};

    template <typename T>
    struct holor_<T> { using type = T; };

    template <typename T, size_t N>
    struct holor_<T, N> { using type = std::array<T, n>; };

    template <typename T, size_t N, size_t ... Ns>
    struct holor_<T, N, Ns...> { using type = std::array<typename holor_<T, Ns>::type, N>; };
  }

  // multi-dimensional array
  template <typename T, size_t ... Shape>
  using holor = typename detail::holor_<T, Shape...>::type;
}