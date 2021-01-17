#pragma once
#include <utility>
#include "tape.h"
#include "shape.h"

namespace bad {

  template <typename B, typename T, typename Act = T*, index_t ... Shape>
  struct expr {
    static constexpr index_t size = prod<Shape...>;
    // the dimensions used
    using shape_t = std::integer_sequence<index_t, Shape...>;
    // a sequence of 0,1,2,3,4 up to dimension.
    using seq_t = std::make_integer_sequence<int, sizeof...(Shape)>;

    template <int N> static constexpr index_t step = stride<N,index_t,Shape...>;
    template <int N> static constexpr index_t dim = nth<N,index_t,Shape...>;

  private:
    template <typename E> struct steps_;
    template <int ... Is> struct steps_<std::integer_sequence<index_t, Is...>> {
      static constexpr index_t value[] = { step<Is> ... };
    };
  public:
    static constexpr index_t steps[] = steps_<std::make_integer_sequence<int, sizeof...(Shape)>>::value;

    static constexpr bool is_simple = B::is_simple;
    // simple should let me know this is just a mapping operation

    // compute the primal value
    template <typename ... Args>
    inline T primal(Args ... args) const {
      static_assert(sizeof...(args) == sizeof...(Shape));
      return static_cast<B const &>(*this).primal(args...);
    }

    template <typename ... Args>
    inline void load(T * data, Args ... args) {
      constexpr index_t N = sizeof...(Args);
      if constexpr (N == sizeof...(Shape)) {
        *data = primal(args...);
      } else {
        constexpr int di = step<N>;
        for (index_t i=0; i<N; ++i, data += di)
          load(data, i, args ...);
      }
    }

    // only available of is_simple says yes
    template <typename = std::enable_if_t<is_simple> >
    void load_simple(T * data) {
      return static_cast<B const &>(*this).load_simple(data);
    }
  };

  template <typename T, typename Act = T*, index_t ... Shape>
  struct ad : expr<ad<T,Act,Shape...>,T,Act,Shape...> {
    using tape_t = tape<T, Act>;
    using shape_t = std::integer_sequence<index_t, Shape...>;
    mutable tape_t * tape;
    index_t index;
    T primals[prod<Shape...>];

    template <typename B>
    ad(expr<B, T, Act, Shape...> const & expr) : primals() {
      if constexpr (B::is_simple) {
        expr.load_simple(primals);
      } else {
        expr.load(primals);
      }
    }

    template <typename ... Args>
    inline T primal(Args ... args) const {
      // TODO:
      return T();
      //      return primals[flat<Shape...>(args...)];
    }
  };
}