#pragma once

#include <cstdint>
#include <utility>
#include "attrib.hh"

namespace bad {
  using std::size_t;
  using std::ptrdiff_t;

  // * sequence types

  template <class T, T... is>
  using seq_t = std::integer_sequence<T, is...>;

  template <auto x, auto... xs>
  using aseq = seq_t<decltype(x), x, xs...>;

  template <size_t... is>
  using seq = seq_t<size_t, is...>;

  template <ptrdiff_t... is>
  using sseq = seq_t<ptrdiff_t, is...>;

  template <char...cs>
  using str = seq_t<char, cs...>;

  // * sequence construction
  //
  template <class T, T x>
  using make_seq_t = std::make_integer_sequence<T,x>;
 
  template <auto x>
  using make_aseq = make_seq_t<decltype(x), x>;
  
  template <size_t x>
  using make_seq = make_seq_t<size_t, x>;

  template <ptrdiff_t x>
  using make_sseq = make_seq_t<ptrdiff_t, x>;

  // * char is an 'integer type'.

#ifdef __clang__
#ifndef ICC // icpc is a lying liar that lies
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-string-literal-operator-template"
#endif
#elif defined __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-string-literal-operator-template"
#endif

  /// until you give me a standard legal way to do this, i'm going to do what i have to do
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

  // * application

  namespace detail {
    /// @private
    template <template <class T, T...> class, class>
    struct seq_apply_;

    /// @private
    template <template <class T, T...> class F, class X, X ... xs>
    struct seq_apply_<F,seq_t<X,xs...>> {
      using type = F<X,xs...>;
    };
  }

  template <template <class T, T...> class F, class L>
  using seq_apply = typename detail::seq_apply_<F,L>::type;

  namespace detail {
    /// @private
    template <template <auto x, decltype(x) ...> class, class>
    struct seq_auto_apply_;

    /// @private
    template <template <auto x, decltype(x) ...> class F, class X, X x, X ... xs>
    struct seq_auto_apply_<F,seq_t<X,x,xs...>> {
      using type = F<x,xs...>;
    };
  }

  template <template <auto x, decltype(x) ...> class F, class L>
  using seq_auto_apply = typename detail::seq_auto_apply_<F,L>::type;

  namespace detail {
    /// @private
    template <class T, template <T...> class, class>
    struct seq_t_apply_;

    /// @private
    template <class T, template <T...> class F, T... xs>
    struct seq_t_apply_<T,F,seq_t<T,xs...>> {
      using type = F<xs...>;
    };
  }

  template <class T, template <T...> class F, class L>
  using seq_t_apply = typename detail::seq_t_apply_<T,F,L>::type;

  // * ranges

  namespace detail {
    /// @private
    template <typename T, T x, typename S>
    struct seq_range_;

    /// @private
    template <typename T, T x, T... ys>
    struct seq_range_<T,x,seq_t<T,ys...>> {
      using type = seq_t<decltype(x),(x+ys)...>;
    };
  }

  template <auto x, decltype(x) y>
  using seq_range = typename detail::seq_range_<decltype(x),x,make_aseq<y-x>>::type;

  namespace detail {
    /// @private
    template <class, class, class>
    struct seq_append__;

    /// @private
    template <class T, T... ts, T... ss>
    struct seq_append__<T,seq_t<T,ts...>, seq_t<T,ss...>> {
      using type = seq_t<T,ts...,ss...>;
    };

    /// @private
    template <class, class...>
    struct seq_append_;

    /// @private
    template <class T>
    struct seq_append_<T> {
      using type = seq_t<T>;
    };

    /// @private
    template <class T, T...ts, class...ss>
    struct seq_append_<T,seq_t<T,ts...>, ss...> {
      using type = typename seq_append__<T,seq_t<T,ts...>,typename seq_append_<T,ss...>::type>::type;
    };
  }
  template <class T, class... S>
  using seq_append = typename detail::seq_append_<T,S...>::type;

  // * reify

  namespace detail {
    /// @private
    template <class>
    struct reify_;

    /// @private
    template <class T, T...xs>
    struct reify_<seq_t<T, xs...>> {
      typedef T const type[sizeof...(xs)];
      static constexpr T const value[sizeof...(xs)] = { xs ... };
    };
  }


  template <class S>
  constexpr typename detail::reify_<S>::type & reify = detail::reify_<S>::value;

  // * products

  template <class T, T... is>
  constexpr T prod_t = (T(1) * ... * is);

  template <auto... xs>
  constexpr auto prod = (...*xs);

  namespace detail {
    /// @private
    template <class>
    struct seq_prod_;

    /// @private
    template <class T>
    struct seq_prod_<seq_t<T>> {
      static constexpr T value = 1;
    };

    /// @private
    template <class T, T i, T... is>
    struct seq_prod_<seq_t<T,i,is...>> {
      static constexpr T value = (i * ... * is);
    };
  }

  template <class S>
  constexpr auto seq_prod = detail::seq_prod_<S>::value;

  // * sums

  template <auto... xs>
  constexpr auto total = (...+xs);

  template <class T, T... xs>
  constexpr T total_t = (T(1)+...+xs);

  namespace detail {
    /// @private
    template <class>
    struct seq_total_;

    /// @private
    template <class T>
    struct seq_total_<seq_t<T>> {
      static constexpr T value = 0;
    };

    /// @private
    template <class T, T i, T... is>
    struct seq_total_<seq_t<T,i,is...>> {
      static constexpr T value = (i + ... + is);
    };
  }

  template <class S>
  constexpr auto seq_total = detail::seq_total_<S>::value;

  // * head

  template <auto x, decltype(x) ... xs>
  constexpr auto head = x;

  namespace detail {
    /// @private
    template <class>
    struct seq_head_;

    /// @private
    template <class T, T ... is>
    struct seq_head_<seq_t<T,is...>> {
      constexpr static T value = head<is...>;
    };
  }

  template <class S>
  constexpr auto seq_head = detail::seq_head_<S>::value;

  // * tail

  template <auto x, decltype(x) ... xs>
  using tail = seq_t<decltype(x), xs...>;

  template <class S>
  using seq_tail = seq_auto_apply<tail,S>;

  // * cons

  namespace detail {
    /// @private
    template <class T, T, class>
    struct seq_cons_;

    /// @private
    template <class T, T i, T ... is>
    struct seq_cons_<T,i,seq_t<T,is...>> {
      using type = seq_t<T,i,is...>;
    };
  }

  template <auto i, class S>
  using seq_cons = typename detail::seq_cons_<decltype(i),i,S>::type;

  // * element type

  namespace detail {
    /// @private
    template <class>
    struct seq_element_type_;

    /// @private
    template <class T, T ... is>
    struct seq_element_type_<seq_t<T,is...>> {
      using type = T;
    };
  };

  template <class S>
  using seq_element_type = typename detail::seq_element_type_<S>::type;

  // * sequence length

  namespace detail {
    /// @private
    template <class>
    struct seq_length_;

    /// @private
    template <class T, T ... is>
    struct seq_length_<seq_t<T,is...>> {
      static constexpr auto value = sizeof...(is);
    };
  }

  template <class S>
  constexpr auto seq_length = detail::seq_length_<S>::value;

  // * row-major stride calculation

  namespace detail {
    /// @private
    template <size_t, size_t...>
    struct stride_;

    /// @private
    template <>
    struct stride_<0> {
      BAD(hd,const) // consteval
      static constexpr ptrdiff_t value() noexcept {
        return 1;
      }
    };

    /// @private
    template <size_t N, size_t x, size_t... xs>
    struct stride_<N,x,xs...> {
      BAD(hd,const) // consteval
      static constexpr ptrdiff_t value() noexcept {
        if constexpr (N == 0) {
          return (ptrdiff_t(1) * ... * ptrdiff_t(xs));
        } else {
          return stride_<N-1,xs...>::value();
        }
      }
    };
  }

  template <size_t N, size_t... xs>
  BAD(constinit)
  constexpr ptrdiff_t stride = detail::stride_<N,xs...>::value();

  namespace detail {
    /// @private
    template <size_t, class>
    struct seq_stride_;

    /// @private
    template <size_t N, size_t ... is>
    struct seq_stride_<N,seq<is...>> {
      static constexpr ptrdiff_t value = stride<N,is...>;
    };
  }

  template <size_t N, class S>
  constexpr ptrdiff_t seq_stride = detail::seq_stride_<N,S>::value;

  namespace detail {
    /// @private
    template <class, class>
    struct row_major_;

    /// @private
    template <class S, size_t... is>
    struct row_major_<S,seq<is...>> {
      using type = sseq<seq_stride<is,S>...>;
    };
  }

  // * compute all row-major strides given a set of dimensions

  template <class S>
  using row_major = typename detail::row_major_<S, make_seq<seq_length<S>>>::type;

  // * indexing

  namespace detail {
    /// @private
    template <size_t N, auto... xs>
    BAD(hd,const)
    constexpr auto nth_() noexcept {
      static_assert(N < sizeof...(xs), "index out of bounds");
      constexpr decltype(head<xs...>) args[] {xs ...};
      return args[N];
    }
  }

  template <size_t N, auto... xs>
  constexpr auto nth = detail::nth_<N,xs...>();

  namespace detail {
    /// @private
    template <size_t, class>
    struct seq_nth_;

    /// @private
    template <size_t N, class T, T... xs>
    struct seq_nth_<N,seq_t<T,xs...>> {
      static constexpr auto value = nth<N,xs...>;
    };
  }

  template <size_t N, class S>
  constexpr auto seq_nth = detail::seq_nth_<N,S>::value;

  template <class S>
  constexpr auto seq_last = seq_nth<seq_length<S>-1,S>;

  // * backpermute packs and sequences

  /// backpermute<seq_t<T,a,b,c,d>,seq<0,3,2,3,1,0>> = seq_t<T,a,d,c,d,b,a>
  template <class S, size_t... is>
  using backpermute = seq_t<seq_element_type<S>, seq_nth<is,S> ...>;

  namespace detail {
    /// @private
    template <class, class>
    struct seq_backpermute_;

    /// @private
    template <class S, class I, I ... is>
    struct seq_backpermute_<S, seq_t<I,is...>> {
      using type = backpermute<S, is...>;
    };
  }

  template <class S, class T>
  using seq_backpermute = typename detail::seq_backpermute_<S,T>::type;

  // * init

  template <class S>
  using seq_init = seq_backpermute<S, make_seq<seq_length<S>-1>>;

  // drop the last N entries
  template <size_t N, class S>
  using seq_drop_last = seq_backpermute<S, make_seq<seq_length<S>-std::max(N, seq_length<S>)>>;

  // * transpose the last two entries in a sequence

  namespace detail {
    /// @private
    template <class T, T...>
    struct pack_transpose_;

    /// @private
    template <class T, T i, T j>
    struct pack_transpose_<T,i,j> {
      using type = seq_t<T,j,i>;
    };

    /// @private
    template <class T, T i, T j, T k, T... ls>
    struct pack_transpose_<T,i,j,k,ls...> {
      using type = seq_cons<i,typename pack_transpose_<T,j,k,ls...>::type>;
    };
  }

  template <class T, T... is>
  using pack_transpose = typename detail::pack_transpose_<T,is...>::type;

  namespace detail {
    /// @private
    template <class>
    struct seq_transpose_;

    /// @private
    template <class T, T i, T j, T ... is>
    struct seq_transpose_<seq_t<T, i, j, is...>> {
      using type = pack_transpose<T, i, j, is...>;
    };
  }

  template <class S>
  using seq_transpose = typename detail::seq_transpose_<S>::type;

  // * skip the nth element

  namespace detail {
    /// @private
    template <size_t, class, class>
    struct seq_skip_nth_;

    /// @private
    template <size_t N, class S, size_t ... ps>
    struct seq_skip_nth_<N,S,seq<ps...>> {
      template <class Suffix>
      struct at;

      template <size_t... ss>
      struct at<seq<ss...>> {
        using type = seq_t<seq_element_type<S>, seq_nth<ps,S>..., seq_nth<ss+N,S>...>;
      };
    };
  }

  template <size_t N, class S>
  using seq_skip_nth = typename detail::seq_skip_nth_<
    N, S, make_seq<N>
  >::template at<
    make_seq<seq_length<S>-1-N>
  >::type;

  // heterogeneous list
  template <class...>
  struct list {};

  // * list cons

  namespace detail {
    /// @private
    template <class, class>
    struct list_cons_;

    /// @private
    template <class x, class... xs>
    struct list_cons_<x,list<xs...>> {
      using type = list<x, xs...>;
    };
  }

  template <class x, class xs>
  using list_cons = typename detail::list_cons_<x,xs>::type;

  // * list head

  namespace detail {
    /// @private
    template <class>
    struct list_head_;

    /// @private
    template <class x, class... xs>
    struct list_head_<list<x,xs...>> {
      using type = x;
    };
  }

  template <class L>
  using list_head = typename detail::list_head_<L>::type;

  // * conversion from sequences

  template <auto x>
  using int_t = std::integral_constant<decltype(x), x>;

  namespace detail {
    /// @private
    template <class>
    struct seq_list_;
  }

  namespace detail {
    /// @private
    template <class T, T... is>
    struct seq_list_<seq_t<T,is...>> {
      using type = list<int_t<is>...>;
    };
  }

  template <class S> using seq_list = typename detail::seq_list_<S>::type;

  // * zipping

  namespace detail {
    /// @private
    template <class>
    struct list_zip_;

    /// @private
    template <>
    struct list_zip_<list<>> {
      template <class>
      struct at {
        using type = list<>;
      };
    };

    /// @private
    template <class x, class... xs>
    struct list_zip_<list<x,xs...>> {
      template <class>
      struct at;

      template <>
      struct at<list<>> {
        using type = list<>;
      };

      template <class y, class... ys>
      struct at<list<y,ys...>> {
        using type = list_cons<std::tuple<x,y>,typename list_zip_<xs...>::template at<ys...>::type>;
      };
    };
  };

  template <class X, class Y>
  using list_zip = typename detail::list_zip_<X>::template at<Y>::type;

  namespace detail {
    /// @private
    template <class, class>
    struct list_seq_;

    /// @private
    template <class T>
    struct list_seq_<T,list<>> {
      using type = seq_t<T>;
    };

    /// @private
    template <class T, T i, class... xs>
    struct list_seq_<T,list<int_t<i>,xs...>> {
      using type = seq_cons<i,typename list_seq_<T,list<xs...>>::type>;
    };
  };

  template <class T, class S>
  using list_seq = typename detail::list_seq_<T,S>::type;

  // list application

  namespace detail {
    /// @private
    template <template <class...> class, class>
    struct list_apply_;

    /// @private
    template <template <class...> class F, class... xs>
    struct list_apply_<F,list<xs...>> {
      using type = F<xs...>;
    };
  }

  template <template <class...> class F, class L>
  using list_apply = typename detail::list_apply_<F,L>::type;
}
