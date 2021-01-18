#pragma once

#include <cstdint>
#include <utility>

namespace bad {
  // possible convenience types
  using diff_t = std::int32_t;
  using index_t = std::uint32_t;
  static constexpr index_t no_index = static_cast<index_t>(-1);
  template <index_t ... is> using seq = std::integer_sequence<index_t, is...>;

  // compute the product of a parameter pack
  template <typename T, T ... xs>
  constexpr auto prod = (1*...*xs);

  // compute the sum of a parameter pack
  template <typename T, T ... xs>
  constexpr auto total = (0+...+xs);

  namespace detail {
    template <std::size_t N, typename T, T ... xs> struct stride_;

    template <typename T>
    struct stride_<0,T> {
      static constexpr T value() { return 1; }
    };

    // compute the nth level stride for an arraw in row-major order.
    template <std::size_t N, typename T, T x, T ... xs>
    struct stride_<N,T,x,xs...> {
      static constexpr T value() {
        if constexpr (N == 0) {
          return prod<T,xs...>;
        } else {
          return stride_<N-1,T,xs...>::value();
        }
      }
    };
  }

  template <std::size_t N, typename T, T ... xs>
  constexpr T stride = detail::stride_<N,T,xs...>::value();

  namespace detail {
    // return the nth item in a parameter pack.
    template <std::size_t N, typename T, T ... xs>
    constexpr T nth_() {
      static_assert(0 < N && N < sizeof...(xs), "index out of bounds");
      constexpr T args[] {xs ...};
      return args[N];
    }
  }

  template <std::size_t N, typename T, T ... xs>
  constexpr T nth = detail::nth_<N,T,xs...>();

  namespace detail {
    template <typename T>
    struct seq_element_type_{};

    template <typename T, T ... is>
    struct seq_element_type_<std::integer_sequence<T,is...>> {
      using type = T;
    };
  };

  // extract the type from an std::integer_sequence
  template <typename S>
  using seq_element_type = typename detail::seq_element_type_<S>::type;

  namespace detail {
    template <typename T>
    struct seq_head_{};

    template <typename T, T i, T ... is>
    struct seq_head_<std::integer_sequence<T,i,is...>> {
      constexpr static T value = i;
    };
  }

  // extract the first element from an std::integer_sequence
  template <typename S>
  constexpr auto seq_head = detail::seq_head_<S>::value;

  namespace detail {
    template <typename S>
    struct seq_tail_{};

    template <typename T, T i, T ... is>
    struct seq_tail_<std::integer_sequence<T,i,is...>> {
      using type = std::integer_sequence<T,is...>;
    };
  }

  // extract the tail of a std::integer_sequence as a new std::integer_sequence
  template <typename S>
  using seq_tail = typename detail::seq_tail_<S>::type;

  namespace detail {
    template <typename S> struct seq_length_{};
    template <typename T, T ... is>
    struct seq_length_<std::integer_sequence<T,is...>> {
      static constexpr auto value = sizeof...(is);;
    };
  }

  // return the length of a std::integer_sequence
  template <typename S>
  constexpr auto seq_length = detail::seq_length_<S>::value;

  namespace detail {
    template <typename S> struct seq_product_{};
    template <typename T, T ... is>
    struct seq_product_<std::integer_sequence<T,is...>> {
      static constexpr auto value = (T(1) * ... * is);
    };
  }

  // compute the product of a std::integer_sequence
  template <typename S>
  constexpr auto seq_product = detail::seq_product_<S>::value;

  namespace detail {
    template <size_t N, typename S> struct seq_stride_{};
    template <size_t N, typename T, T ... is>
    struct seq_stride_<N,std::integer_sequence<T,is...>> {
      static constexpr auto value = stride<N,T,is...>;
    };
  }

  // grab the stride of the nth dimension given row-major dimensions as a std::integer_sequence.
  template <size_t N, typename S>
  constexpr auto seq_stride = detail::seq_stride_<N,S>::value;

  namespace detail {
    template <typename S, typename Is> struct row_major_;
    template <typename S, typename T, T ... is>
    struct row_major_<S,std::integer_sequence<T,is...>> {
      using type = std::integer_sequence<T,(seq_stride<is,S>)...>;
    };
  }

  // compute strides in row major order for a sequence of dimensions.
  template <typename S>
  using row_major = typename detail::row_major_<
    S,
    typename std::make_index_sequence<seq_length<S>>
  >::type;
}