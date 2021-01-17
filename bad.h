#pragma once
#include "tape.h"
#include "shape.h"

namespace bad {

  template <typename T, typename ... Ts>
  constexpr T product(T x, Ts ... xs) {
    return x * ... * xs;
  }
  template <typename T> constexpr T product() {
    return 1;
  }



  template <typename B, typename T, typename Act = T*, index_t ... Shape>
  struct expr {
    static constexpr size = product(Shape...);

    template <typename ... Args> T primal(Args ... args) {
      static_assert(sizeof...(args) == sizeof...(Shape));
      return static_cast<B const &>(*this).primal(args...);
    }
  };


  template <typename T, typename Act = T*, index_t ... Shape>
  struct ad : expr<ad<T,Act,Shape...>,T,Act,Shape...> {
    using tape_t = tape<T, Act>;
    mutable tape_t * tape;
    index_t index;
    T primals[size];
    template <typename E>
    ad(expr<B, T, Act, > const & expr) : elems() {

    }


    ad() noexcept
    : primal()
    , index(no_index)
    , tape() {}

    ad(ad && rhs) noexcept
    : primal(std::move_if_noexcept(rhs.primal))
    , index(std::move(rhs.index))
    , tape(rhs.tape) {}

    ad(const ad & rhs) noexcept
    : primal(rhs.primal)
    , index(rhs.index)
    , tape(rhs.tape)
    {}

    ad(const T & primal) noexcept : primal(primal), tape(), index(no_index) {}
    ad(const T & primal, tape_t * tape, index_t index) noexcept : primal(primal), tape(tape), index(index) {}


    // and we want ad_exprs that can generate a whole expression at a time as a single ad record on the tape.
  };
}