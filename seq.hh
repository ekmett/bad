#pragma once

#include <cstdint>
#include <utility>

namespace bad {
  using std::size_t;
  using std::ptrdiff_t;

  // * sequence types

  template <class T, T... is> using seq_t = std::integer_sequence<T, is...>;
  template <auto x, auto... xs> using seq = seq_t<decltype(x), x, xs...>;
  template <size_t ... is> using iseq = std::index_sequence<is...>;
  template <char...cs> using str = std::integer_sequence<char, cs...>;

  // * sequence construction

  template <auto x> using make_seq = std::make_integer_sequence<decltype(x),x>;

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

  // until you give me a standard legal way to do this, i'm going to do what i have to do
  template <class T, T...cs>
  str<cs...> operator""_str() {
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
    template <template <class T, T...> class, class> struct seq_apply_;
    template <template <class T, T...> class F, class X, X ... xs> struct seq_apply_<F,seq_t<X,xs...>> {
      using type = F<X,xs...>;
    };
  }
  template <template <class T, T...> class F, class L> using seq_apply = typename detail::seq_apply_<F,L>::type;

  namespace detail {
    template <template <auto x, decltype(x) ...> class, class> struct seq_auto_apply_;
    template <template <auto x, decltype(x) ...> class F, class X, X x, X ... xs> struct seq_auto_apply_<F,seq_t<X,x,xs...>> {
      using type = F<x,xs...>;
    };
  }
  template <template <auto x, decltype(x) ...> class F, class L> using seq_auto_apply = typename detail::seq_auto_apply_<F,L>::type;

  namespace detail {
    template <class T, template <T...> class, class> struct seq_t_apply_;
    template <class T, template <T...> class F, T... xs> struct seq_t_apply_<T,F,seq_t<T,xs...>> {
      using type = F<xs...>;
    };
  }

  template <class T, template <T...> class F, class L> using seq_t_apply = typename detail::seq_t_apply_<T,F,L>::type;

  // * ranges

  namespace detail {
    template <typename T, T x, typename S> struct seq_range_;
    template <typename T, T x, T ... ys> struct seq_range_<T,x,seq_t<T,ys...>> {
      using type = seq_t<decltype(x),(x+ys)...>;
    };
  }

  template <auto x, decltype(x) y>
  using seq_range = typename detail::seq_range_<decltype(x),x,make_seq<y-x>>::type;

  // * reify

  namespace detail {
    template <class> struct reify_;
    template <class T, T...xs> struct reify_<seq_t<T, xs...>> {
       typedef T const type[sizeof...(xs)];
       static constexpr T const value[sizeof...(xs)] = { xs ... };
    };
  }

  template <class S> constexpr typename detail::reify_<S>::type & reify = detail::reify_<S>::value;

  // * products

  template <class T, T... is> constexpr T prod_t = (T(1) * ... * is);
  template <auto... xs> constexpr auto prod = (...*xs);

  namespace detail {
    template <class> struct seq_prod_;
    template <class T> struct seq_prod_<seq_t<T>> {
      static constexpr T value = 1;
    };
    template <class T, T i, T... is> struct seq_prod_<seq_t<T,i,is...>> {
      static constexpr T value = (i * ... * is);
    };
  }

  template <class S> constexpr auto seq_prod = detail::seq_prod_<S>::value;

  // * sums

  template <auto... xs> constexpr auto total = (...+xs);
  template <class T, T... xs> constexpr T total_t = (T(1)+...+xs);

  namespace detail {
    template <class> struct seq_total_;
    template <class T> struct seq_total_<seq_t<T>> {
      static constexpr T value = 0;
    };
    template <class T, T i, T... is> struct seq_total_<seq_t<T,i,is...>> {
      static constexpr T value = (i + ... + is);
    };
  }

  template <class S> constexpr auto seq_total = detail::seq_total_<S>::value;

  // * head

  template <auto x, decltype(x) ... xs> constexpr auto head = x;

  namespace detail {
    template <class> struct seq_head_;
    template <class T, T ... is> struct seq_head_<seq_t<T,is...>> {
      constexpr static T value = head<is...>;
    };
  }

  template <class S> constexpr auto seq_head = detail::seq_head_<S>::value;

  // * tail

  template <auto x, decltype(x) ... xs> using tail = seq_t<decltype(x), xs...>;

  template <class S> using seq_tail = seq_auto_apply<tail,S>;

  // * cons

  namespace detail {
    template <class T, T, class> struct seq_cons_;
    template <class T, T i, T ... is> struct seq_cons_<T,i,seq_t<T,is...>> {
      using type = seq_t<T,i,is...>;
    };
  }

  template <auto i, class S>
  using seq_cons = typename detail::seq_cons_<decltype(i),i,S>::type;

  // * element type

  namespace detail {
    template <class> struct seq_element_type_;
    template <class T, T ... is> struct seq_element_type_<seq_t<T,is...>> {
      using type = T;
    };
  };

  template <class S> using seq_element_type = typename detail::seq_element_type_<S>::type;

  // * sequence length

  namespace detail {
    template <class> struct seq_length_;
    template <class T, T ... is> struct seq_length_<seq_t<T,is...>> {
      static constexpr auto value = sizeof...(is);
    };
  }

  template <class S> constexpr auto seq_length = detail::seq_length_<S>::value;

  // * row-major stride calculation

  namespace detail {
    template <size_t, class T, T...> struct stride_;
    template <class T> struct stride_<0,T> {
      // consteval?
      static constexpr T value() noexcept { return 1; }
    };
    template <size_t N, class T, T x, T... xs>
    struct stride_<N,T,x,xs...> {
      // consteval?
      static constexpr decltype(x) value() noexcept {
        if constexpr (N == 0) {
          return (T(1) * ... * xs); // use prod_t in case i choose to manually drop the T(1) fold?
        } else {
          return stride_<N-1,T,xs...>::value();
        }
      }
    };
  }

  // constinit?
  template <size_t N, class T, T... xs> constexpr auto stride = detail::stride_<N,T,xs...>::value();

  namespace detail {
    template <size_t, class> struct seq_stride_;
    template <size_t N, class T, T ... is> struct seq_stride_<N,seq_t<T,is...>> {
      static constexpr auto value = stride<N,T,is...>;
    };
  }

  template <size_t N, class S> constexpr auto seq_stride = detail::seq_stride_<N,S>::value;

  namespace detail {
    template <class, class> struct row_major_;
    template <class S, class T, T... is>
    struct row_major_<S,seq_t<T,is...>> {
      using type = seq_t<T,seq_stride<is,S>...>;
    };
  }

  // * all row-major strides

  template <class S> using row_major = typename detail::row_major_<S, make_seq<seq_length<S>>>::type;

  // * indexing

  namespace detail {
    // return the nth item in a parameter pack.
    template <size_t N, auto... xs>
    constexpr auto nth_() noexcept {
      static_assert(N < sizeof...(xs), "index out of bounds");
      constexpr decltype(head<xs...>) args[] {xs ...};
      return args[N];
    }
  }

  template <size_t N, auto... xs> constexpr auto nth = detail::nth_<N,xs...>();

  namespace detail {
    template <size_t, class> struct seq_nth_;
    template <size_t N, class T, T... xs> struct seq_nth_<N,seq_t<T,xs...>> {
      static constexpr auto value = nth<N,xs...>;
    };
  }

  template <size_t N, class S> constexpr auto seq_nth = detail::seq_nth_<N,S>::value;

  template <class S> constexpr auto seq_last = seq_nth<seq_length<S>-1,S>;

  // * backpermute packs and sequences

  // backpermute<seq<a,b,c,d>,iseq<0,3,2,3,1,0>> = seq<a,d,c,d,b,a>
  template <class S, size_t... is>
  using backpermute = seq_t<seq_element_type<S>, seq_nth<is,S> ...>;

  namespace detail {
    template <class, class> struct seq_backpermute_;
    template <class S, class I, I ... is> struct seq_backpermute_<S, seq_t<I,is...>> {
      using type = backpermute<S, is...>;
    };
  }

  template <class S, class T> using seq_backpermute = typename detail::seq_backpermute_<S,T>::type;

  // * init

  template <class S> using seq_init = seq_backpermute<S, make_seq<seq_length<S>-1>>;

  // drop the last N entries
  template <size_t N, class S>
  using seq_drop_last = seq_backpermute<S, make_seq<seq_length<S>-std::max(N, seq_length<S>)>>;

  // * transpose the last two entries in a sequence

  namespace detail {
    template <class T, T ...> struct pack_transpose_;
    template <class T, T i, T j> struct pack_transpose_<T,i,j> {
      using type = seq_t<T,j,i>;
    };
    template <class T, T i, T j, T k, T ... ls> struct pack_transpose_<T,i,j,k,ls...> {
      using type = seq_cons<i,typename pack_transpose_<T,j,k,ls...>::type>;
    };

    // maybe move out of detail
    template <class T, T ... is> using pack_transpose = typename detail::pack_transpose_<T,is...>::type;

    template <class> struct seq_transpose_;
    template <class T, T i, T j, T ... is> struct seq_transpose_<seq_t<T, i, j, is...>> {
      using type = pack_transpose<T, i, j, is...>;
    };
  }

  template <class S>
  using seq_transpose = typename detail::seq_transpose_<S>::type;

  // * skip the nth element

  namespace detail {
    template <size_t, class, class> struct seq_skip_nth_;
    template <size_t N, class S, size_t ... ps> struct seq_skip_nth_<N,S,iseq<ps...>> {
      template <class Suffix> struct at;
      template <size_t ... ss> struct at<iseq<ss...>> {
        using type = seq_t<seq_element_type<S>, seq_nth<ps,S>..., seq_nth<ss+N,S>...>;
      };
    };
  }

  template <size_t N, class S>
  using seq_skip_nth = typename detail::seq_skip_nth_<N,S,make_seq<N>>::template at<make_seq<seq_length<S>-1-N>>::type;

  // heterogeneous list
  template <class...> struct list {};

  // * list cons

  namespace detail {
    template <class, class> struct list_cons_;
    template <class x, class... xs> struct list_cons_<x,list<xs...>> {
      using type = list<x, xs...>;
    };
  }

  template <class x, class xs> using list_cons = typename detail::list_cons_<x,xs>::type;

  // * list head

  namespace detail {
    template <class> struct list_head_;
    template <class x, class... xs> struct list_head_<list<x,xs...>> {
      using type = x;
    };
  }

  template <class L> using list_head = typename detail::list_head_<L>::type;

  // * conversion from sequences

  template <auto x> using int_t = std::integral_constant<decltype(x), x>;

  namespace detail {
    template <class> struct seq_list_;
    template <class T, T... is> struct seq_list_<seq_t<T,is...>> {
      using type = list<int_t<is>...>;
    };
  }

  template <class S> using seq_list = typename detail::seq_list_<S>::type;

  // * zipping

  namespace detail {
    template <class> struct list_zip_;
    template <> struct list_zip_<list<>> {
      template <class> struct at {
        using type = list<>;
      };
    };
    template <class x, class... xs> struct list_zip_<list<x,xs...>> {
      template <class> struct at;
      template <> struct at<list<>> {
        using type = list<>;
      };
      template <class y, class... ys> struct at<list<y,ys...>> {
        using type = list_cons<std::tuple<x,y>,typename list_zip_<xs...>::template at<ys...>::type>;
      };
    };
  };

  template <class X, class Y> using list_zip = typename detail::list_zip_<X>::template at<Y>::type;

  namespace detail {
    template <class, class> struct list_seq_;
    template <class T> struct list_seq_<T,list<>> {
      using type = seq_t<T>;
    };
    template <class T, T i, class... xs> struct list_seq_<T,list<int_t<i>,xs...>> {
      using type = seq_cons<i,typename list_seq_<T,list<xs...>>::type>;
    };
  };

  template <class T, class S> using list_seq = typename detail::list_seq_<T,S>::type;

  // list application

  namespace detail {
    template <template <class...> class, class> struct list_apply_;
    template <template <class...> class F, class... xs> struct list_apply_<F,list<xs...>> {
      using type = F<xs...>;
    };
  }

  template <template <class...> class F, class L> using list_apply = typename detail::list_apply_<F,L>::type;


}
