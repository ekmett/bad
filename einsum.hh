#pragma once
#include <string>
#include <cstdint>
#include "store.hh"
#include "seq.hh"
namespace bad {
/*
  // an interned compile time string with o(1) comparison.
  template <char...cs> struct symbol {
    static constexpr char value[sizeof...(cs)+1] = { cs..., '\0'};
    static symbol<cs...> instance;
    static constexpr size_t size = sizeof...(cs);
    operator std::string () const { return value; }
    operator char const * () const { return value; }
  }; // a compile time string holding a parameter pack.
*/

  // who knows, it varies by compiler?
  //template <char...cs>

/*

  template <char...cs>
  symbol<cs...> symbol<cs...>::instance = {};

  // comma separate a symbol
  namespace detail {
    template <std::size_t Acc, typename T, T X, T ... Ys>
    struct find_ {};

    template <std::size_t Acc, typename T, T X>
    struct find_<Acc,T,X> {
      static constexpr std::size_t value() {
       return Acc;
      }
    };

    template <std::size_t Acc, typename T, T X, T Y, T ... Ys>
    struct find_<Acc,T,X,Y,Ys...> {
      static constexpr std::size_t value() {
        if constexpr (X == Y) {
          return Acc;
        } else {
          return find_<Acc+1,T,X,Ys...>::value();
        }
      }
    };
  }

*/
}
