#pragma once

namespace bad {
  // stride<0>(7,5,3) = 1
  // stride<1>(7,5,3) = 7
  // stride<2>(7,5,3) = 35
  // stride<3>(7,5,3) = 105

  template <int N, typename T, typename ... Ts>
  constexpr T stride(T x, Ts ... xs) {
    static_assert(N <= 1 + sizeof...(xs), "index out of bounds");
    T args[] = { x, xs ... };
    T acc = 1;
    for (int i=0;i<N;++i) {
      acc *= args[i];
    }
    return acc;
  }
  template <int N>
  constexpr auto stride() {
    static_assert(N==0, "index out of bounds");
    return 1;
  }

  template <typename ... Ts>
  constexpr auto product(Ts ... xs) {
    return (1*...*xs);
  }

  template <typename ... Ts>
  constexpr auto sum(Ts ... xs) {
    return (0+...+xs);
  }


}