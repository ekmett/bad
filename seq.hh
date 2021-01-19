#pragma once

#include <cstdint>
#include <utility>

namespace bad {
  // possible convenience types
  using diff_t = std::int32_t;
  using index_t = std::uint32_t;
  static constexpr index_t no_index = static_cast<index_t>(-1);

  template <index_t ... is>
  using seq = std::integer_sequence<index_t, is...>;

  namespace detail {
    template <typename S> struct reify_{};
    template <typename T, T...xs> struct reify_<std::integer_sequence<T, xs...>> {
       typedef T const type[sizeof...(xs)];
       static constexpr T const value[sizeof...(xs)] = { xs ... };
    };
  }

  // convert to a user 
  template <typename S>
  constexpr typename detail::reify_<S>::type & reify = detail::reify_<S>::value;

  // auto sequence type
  template <auto x, auto ... xs>
  using aseq = std::integer_sequence<decltype(x), x, xs...>;


  template <auto ... xs>
  constexpr auto prod = (1*...*xs);

  // compute the sum of a parameter pack
  template <auto ... xs>
  constexpr auto total = (0+...+xs);

  template <auto x, decltype(x) ... xs>
  constexpr auto head = x;

  template <auto x, decltype(x) ... xs>
  using tail = std::integer_sequence<decltype(x), xs...>;

  namespace detail {
    template <std::size_t N, auto ... xs> struct stride_;

    template <>
    struct stride_<0> {
      static constexpr auto value() { return 1; } // TODO: smallest growable type, uint8_t?
    };

    // compute the nth level stride for an arraw in row-major order.
    template <std::size_t N, auto x, decltype(x) ... xs>
    struct stride_<N,x,xs...> {
      static constexpr decltype(x) value() {
        if constexpr (N == 0) {
          return prod<xs...>;
        } else {
          return stride_<N-1,xs...>::value();
        }
      }
    };
  }

  template <std::size_t N, auto ... xs>
  constexpr auto stride = detail::stride_<N,xs...>::value();

  namespace detail {
    // return the nth item in a parameter pack.
    template <std::size_t N, auto ... xs>
    constexpr auto nth_() {
      static_assert(0 < N && N < sizeof...(xs), "index out of bounds");
      constexpr decltype(head<xs...>) args[] {xs ...};
      return args[N];
    }
  }

  template <std::size_t N, auto ... xs>
  constexpr auto nth = detail::nth_<N,xs...>();

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

    template <typename T, T ... is>
    struct seq_head_<std::integer_sequence<T,is...>> {
      constexpr static T value = head<is...>;
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
      static constexpr auto value = stride<N,is...>;
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

  namespace detail {
    template <std::size_t N, typename S> struct seq_nth_;
    template <std::size_t N, typename T, T ... xs> struct seq_nth_<N,std::integer_sequence<T,xs...>> {
      static constexpr auto value = nth<N,xs...>;
    };
  }
  template <std::size_t N, typename S>
  constexpr auto seq_nth = detail::seq_nth_<N,S>::value;

  template <typename S>
  constexpr auto seq_last = seq_nth<seq_length<S>-1,S>;

  namespace detail {
    template <typename S, typename Is> struct seq_init_;
    template <typename S, typename T, T... is>
    struct seq_init_<S,std::integer_sequence<T,is...>> {
      using type = std::integer_sequence<seq_element_type<S>,seq_nth<is,S>...>;
    };
  }

  // enable_if seq_length<S> > 1 ?
  template <typename S>
  using seq_init = typename detail::seq_init_<
    S,
    typename std::make_index_sequence<seq_length<S>-1>
  >::type;

  template <std::size_t N, typename S>
  using seq_drop_last = typename detail::seq_init_<
    S,
    typename std::make_index_sequence<seq_length<S>-std::max(N, seq_length<S>)>
  >::type;

  namespace detail {
    template <typename T, T i, typename S> struct seq_cons_{};
    template <typename T, T i, T ... is> struct seq_cons_<T,i,std::integer_sequence<T,is...>> {
      using type = std::integer_sequence<T,i,is...>;
    };
  }
  template <typename T, T i, typename S> using seq_cons = typename detail::seq_cons_<T,i,S>::type;
  
  namespace detail {
    template <typename T, T ... is> struct pack_transpose_;
    template <typename T, T i, T j> struct pack_transpose_<T,i,j> {
      using type = std::integer_sequence<T,j,i>;
    };
    template <typename T, T i, T j, T k, T ... ls> struct pack_transpose_<T,i,j,k,ls...> {
      using type = seq_cons<T,i,typename pack_transpose_<T,j,k,ls...>::type>;
    };
  }

  template <typename T, T ... is> using pack_transpose = typename detail::pack_transpose_<T,is...>::type;

  namespace detail {
    template <typename S> struct seq_transpose_ {};
    template <typename T, T i, T j, T ... is> struct seq_transpose_<std::integer_sequence<T, i, j, is...>> {
      using type = pack_transpose<T, i, j, is...>;
    };
  }

  template <typename S> using seq_transpose = typename detail::seq_transpose_<S>::type;

  /*
  // transpoe the last two rows, used to transpose in place.
  namespace detail {
    template <typename S> struct seq_transpose_ {};
    template <typename T, T i, T j, T ... is> struct seq_transpose_<std::integer_sequence<T,is...,i,j>> {
      using type = std::integer_sequence<T,is...,j,i>;
    };
  }

  template <typename S> using seq_transpose = typename seq_transpose_<S>::type;
  */

}
