#pragma once

#include <cstdint>
#include <utility>

namespace bad {
  // possible convenience types
  //using diff_t = std::int32_t;
  //using index_t = std::uint32_t;
  //static constexpr index_t no_index = static_cast<index_t>(-1);

  template <typename T, T ... is>
  using seq_t = std::integer_sequence<T, is...>;

  // auto sequence type, inference works as long as this list is non-empty
  template <auto x, auto ... xs>
  using seq = seq_t<decltype(x), x, xs...>;

  template <auto x, decltype(x) ... xs>
  constexpr auto head = x;

  template <auto x, decltype(x) ... xs>
  using tail = seq_t<decltype(x), xs...>;

  namespace detail {
    template <typename S> struct reify_{};
    template <typename T, T...xs> struct reify_<seq_t<T, xs...>> {
       typedef T const type[sizeof...(xs)];
       static constexpr T const value[sizeof...(xs)] = { xs ... };
    };
  }

  // convert to a user 
  template <typename S>
  constexpr typename detail::reify_<S>::type & reify = detail::reify_<S>::value;

  template <typename T, T ... is>
  constexpr T prod_t = (T(1) * ... * is);

  // inference fails for prod<>. use prod_t<T> instead.
  template <auto ... xs>
  constexpr auto prod = (...*xs);

  namespace detail {
    template <typename S> struct seq_prod_ {};
    template <typename T> struct seq_prod_<seq_t<T>> {
      static constexpr T value = 1;
    };
    template <typename T, T i, T ... is> struct seq_prod_<seq_t<T,i,is...>> {
      static constexpr T value = (i * ... * is);
    };
  }

  template <typename S> 
  constexpr auto seq_prod = detail::seq_prod_<S>::value;

  // compute the sum of a parameter pack
  // inference fails for <>, use total_t instead
  template <auto ... xs>
  constexpr auto total = (...+xs);

  template <typename T, T ... xs> 
  constexpr T total_t = (T(1)+...+xs);

  namespace detail {
    template <typename S> struct seq_total_ {};
    template <typename T> struct seq_total_<seq_t<T>> {
      static constexpr T value = 0;
    };
    template <typename T, T i, T ... is> struct seq_total_<seq_t<T,i,is...>> {
      static constexpr T value = (i + ... + is);
    };
  }

  template <typename S> 
  constexpr auto seq_total = detail::seq_total_<S>::value;

  namespace detail {
    template <std::size_t N, typename T, T ... xs> struct stride_;

    template <typename T>
    struct stride_<0,T> {
      static constexpr T value() { return 1; }
    };

    // compute the nth level stride for an arraw in row-major order.
    template <std::size_t N, typename T, T x, T ... xs>
    struct stride_<N,T,x,xs...> {
      static constexpr decltype(x) value() {
        if constexpr (N == 0) {
          return (T(1) * ... * xs);
        } else {
          return stride_<N-1,T,xs...>::value();
        }
      }
    };
  }

  template <std::size_t N, typename T, T ... xs>
  constexpr auto stride = detail::stride_<N,T,xs...>::value();

  namespace detail {
    // return the nth item in a parameter pack.
    template <std::size_t N, auto ... xs>
    constexpr auto nth_() {
      static_assert(N < sizeof...(xs), "index out of bounds");
      constexpr decltype(head<xs...>) args[] {xs ...};
      return args[N];
    }
  }

  template <std::size_t N, auto ... xs>
  constexpr auto nth = detail::nth_<N,xs...>();

  // provides nicer error messages than something like
  //template <std::size_t N, auto ... xs>
  //constexpr auto nth = reify<aseq<xs...>>[N];

  namespace detail {
    template <typename T>
    struct seq_element_type_{};

    template <typename T, T ... is>
    struct seq_element_type_<seq_t<T,is...>> {
      using type = T;
    };
  };

  // extract the type from a seq_t
  template <typename S>
  using seq_element_type = typename detail::seq_element_type_<S>::type;

  namespace detail {
    template <typename T>
    struct seq_head_{};

    template <typename T, T ... is>
    struct seq_head_<seq_t<T,is...>> {
      constexpr static T value = head<is...>;
    };
  }

  // extract the first element from a seq_t
  template <typename S>
  constexpr auto seq_head = detail::seq_head_<S>::value;

  namespace detail {
    template <typename S>
    struct seq_tail_{};

    template <typename T, T i, T ... is>
    struct seq_tail_<seq_t<T,i,is...>> {
      using type = seq_t<T,is...>;
    };
  }

  // extract the tail of a seq_t as a new seq_t
  template <typename S>
  using seq_tail = typename detail::seq_tail_<S>::type;

  namespace detail {
    template <typename S> struct seq_length_{};
    template <typename T, T ... is>
    struct seq_length_<seq_t<T,is...>> {
      static constexpr auto value = sizeof...(is);;
    };
  }

  // return the length of a seq_t
  template <typename S>
  constexpr auto seq_length = detail::seq_length_<S>::value;

  namespace detail {
    template <typename S> struct seq_product_{};
    template <typename T, T ... is>
    struct seq_product_<seq_t<T,is...>> {
      static constexpr auto value = (T(1) * ... * is);
    };
  }

  // compute the product of a seq_t
  template <typename S>
  constexpr auto seq_product = detail::seq_product_<S>::value;

  namespace detail {
    template <size_t N, typename S> struct seq_stride_{};
    template <size_t N, typename T, T ... is>
    struct seq_stride_<N,seq_t<T,is...>> {
      static constexpr auto value = stride<N,T,is...>;
    };
  }

  // grab the stride of the nth dimension given row-major dimensions as a seq_t.
  template <size_t N, typename S>
  constexpr auto seq_stride = detail::seq_stride_<N,S>::value;

  namespace detail {
    template <typename S, typename Is> struct row_major_;
    template <typename S, typename T, T ... is>
    struct row_major_<S,seq_t<T,is...>> {
      using type = seq_t<T,(seq_stride<is,S>)...>;
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
    template <std::size_t N, typename T, T ... xs> struct seq_nth_<N,seq_t<T,xs...>> {
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
    struct seq_init_<S,seq_t<T,is...>> {
      using type = seq_t<seq_element_type<S>,seq_nth<is,S>...>;
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

  // backpermute<seq<a,b,c,d>,<0,3,2,3,1,0>> = seq<a,d,c,d,b,a>
  template <typename S, std::size_t ... is> 
  using backpermute = seq_t<seq_element_type<S>, seq_nth<is,S> ...>;

  namespace detail {
    template <typename S, typename T>
    struct seq_backpermute_ {};

    template <typename S, typename I, I ... is>
    struct seq_backpermute_<S, seq_t<I,is...>> {
      using type = backpermute<S, is...>;
    };
  }

  template <typename S, typename T>
  using seq_backpermute = typename detail::seq_backpermute_<S,T>::type;

  namespace detail {
    template <typename T, T i, typename S> struct seq_cons_{};
    template <typename T, T i, T ... is> struct seq_cons_<T,i,seq_t<T,is...>> {
      using type = seq_t<T,i,is...>;
    };
  }

  template <typename T, T i, typename S>
  using seq_cons = typename detail::seq_cons_<T,i,S>::type;

  namespace detail {
    template <typename T, T ... is> struct pack_transpose_;
    template <typename T, T i, T j> struct pack_transpose_<T,i,j> {
      using type = seq_t<T,j,i>;
    };
    template <typename T, T i, T j, T k, T ... ls> struct pack_transpose_<T,i,j,k,ls...> {
      using type = seq_cons<T,i,typename pack_transpose_<T,j,k,ls...>::type>;
    };
  }

  template <typename T, T ... is> using pack_transpose = typename detail::pack_transpose_<T,is...>::type;

  namespace detail {
    template <typename S> struct seq_transpose_ {};
    template <typename T, T i, T j, T ... is> struct seq_transpose_<seq_t<T, i, j, is...>> {
      using type = pack_transpose<T, i, j, is...>;
    };
  }

  template <typename S>
  using seq_transpose = typename detail::seq_transpose_<S>::type;


  /*
  // transpoe the last two rows, used to transpose in place.
  namespace detail {
    template <typename S> struct seq_transpose_ {};
    template <typename T, T i, T j, T ... is> struct seq_transpose_<seq_t<T,is...,i,j>> {
      using type = seq_t<T,is...,j,i>;
    };
  }

  template <typename S> using seq_transpose = typename seq_transpose_<S>::type;
  */

}
