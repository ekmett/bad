#pragma once
#include "store.hh"
#include <string>
namespace bad {

  // an interned compile time string with o(1) comparison.
  template <char...cs> struct sym {
    static constexpr char value[sizeof...(cs)+1] = { cs..., '\0'};
    static sym<cs...> instance;
    static constexpr size_t size = sizeof...(cs);
    operator std::string () const { return value; }
    operator char const * () const { return value; }
  }; // a compile time string holding a parameter pack.

  // who knows, it varies by compiler?
  //template <char...cs>
  template <typename T, T...cs>
  auto operator""_sym() -> sym<cs...> & {
    return sym<cs...>::instance;
  }

  template <char...cs>
  sym<cs...> sym<cs...>::instance = {};

  // comma separate a sym
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

  template <typename T, T X, T ... Ys>
  constexpr std::size_t find = detail::find_<0,T,X,Ys...>::value();

  namespace detail {
    template <char N, typename S>
    struct sym_find_{};

    template <char N, char ... Cs>
    struct sym_find_<N,sym<Cs...>> {
      static constexpr std::size_t value = find<char,N,Cs...>;
    };
  }

  template <char N, typename S>
  constexpr std::size_t sym_find = detail::sym_find_<N,S>::value;

  // namespace detail {
  //   template <typename S> struct sym_len_{

  //   };
  //   template <char...cs> struct sym_len_<str<cs...>> {
  //     static constexpr size_t value = sizeof...(cs);
  //   };
  // };

  // template <typename S>
  // constexpr auto str_len = typename detail::sym_len_<S>::value;

}