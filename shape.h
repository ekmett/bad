#pragma once

namespace bad {
  template <typename T, T ... xs>
  constexpr auto index();

  template <typename T, T ... xs>
  constexpr auto prod = (1*...*xs);

  template <typename T, T ... xs>
  constexpr auto total = (0+...+xs);

  namespace detail {
    template <int N, typename T, T ... xs>
    struct stride_;

    template <typename T>
    struct stride_<0,T> {
      static constexpr T value() { return 1; }
    };

    template <int N, typename T, T x, T ... xs>
    struct stride_<N,T,x,xs...> {
      static constexpr T value() {
        if constexpr (N == 0) {
          return prod<T,xs...>;
        } else {
          return stride_<N-1,T,xs...>::value();
        }
      }
      // stride_<N-1,T,xs...>::value;
    };
  }

  // stride<0,int,7,5,3>() = 15
  // stride<1,int,7,5,3>() = 3
  // stride<2,int,7,5,3>() = 1

  template <int N, typename T, T ... xs>
  constexpr T stride = detail::stride_<N,T,xs...>::value();

  template <int N, typename T, T ... xs>
  constexpr T nth() {
    static_assert(0 < N && N < sizeof...(xs), "index out of bounds");
    constexpr T args[] {xs ...};
    return args[N];
  }

  // template <int N, typename T, typename ... Ts>
  // constexpr auto nth(T x, Ts ... xs) {
  //   T args[] = { x, xs ... };
  //   if constexpr (N == 0) {
  //     return T;
  //   } else {
  //     return nth()
  //   }
  // }



}