#pragma once
#include <string>
#include <cstdint>
#include "store.hh"
namespace bad {

  class symbol {
  private:
    const char * const p;
    const std::size_t sz;
  public:
    using iterator = char const *;
    using const_iterator = char const *;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<iterator>;
    template <std::size_t N> 
    constexpr symbol(const char(&a)[N]) const noexcept : p(a), sz(N-1) {}

    constexpr char at(std::size_t n) const noexcept {
      return n < sz ? p[n] : throw std::out_of_range("");
    }

    constexpr char operator[](std::size_t n) const noexcept {
      return n < sz ? p[n] : throw std::out_of_range("");
    }
    constexpr bool empty() const noexcept { return sz == 0; }
    constexpr std::size_t size() const noexcept { return sz; } 
    //constexpr in c++20;
    operator std::string () const noexcept { return std::string(p, p + sz); }
    constexpr symbol take(std::size_t n) const noexcept { return symbol(p,std::min(n,sz)); }
    constexpr symbol drop(std::size_t n) const noexcept { return symbol(p+std::max(n,sz)); }
    constexpr bool operator == (const symbol & rhs) const noexcept {
      if (size() != rhs.size()) return false;
      for (int i=0;i<sz;++i)
        if (at(i) != rhs.at(i)) return false;
      return true;
    }
    constexpr const_iterator begin() const noexcept { return p; }
    constexpr const_iterator end() const noexcept { return p + sz; }
    constexpr const_iterator cbegin() const noexcept { return p; }
    constexpr const_iterator cend() const noexcept { return p + sz; }
    constexpr const_reverse_iterator rbegin() const noexcept { return std::reverse_iterator(p + sz - 1); }
    constexpr const_reverse_iterator rend() const noexcept { return std::reverse_iterator(p - 1); }
    constexpr const_reverse_iterator crbegin() const noexcept { return std::reverse_iterator(p + sz - 1); }
    constexpr const_reverse_iterator crend() const noexcept { return std::reverse_iterator(p - 1); }
    constexpr bool contains(char c) const noexcept {
      for (std::size_t i=0;i<sz;++i)
        if (at(i) == c) return true;
      return false;
    }
    constexpr bool contains(symbol c) const noexcept {
      for (std::size_t i=0;i<sz;++i)
        if (drop(i).take(rhs.size()) == rhs) return true;
      return false;
    }
    constexpr std::size_t index(char c) const noexcept {
      for (std::size_t i=0;i<sz;++i)
        if (at(i) == c) return i;
      return sz;
    }
    constexpr std::size_t index(symbol c) const noexcept {
      for (std::size_t i=0;i<sz;++i)
        if (drop(i).take(rhs.size()) == rhs) return i;
      return false;
    }
  };

/*

  // an interned compile time string with o(1) comparison.
  template <char...cs> struct symbol {
    static constexpr char value[sizeof...(cs)+1] = { cs..., '\0'};
    static symbol<cs...> instance;
    static constexpr size_t size = sizeof...(cs);
    operator std::string () const { return value; }
    operator char const * () const { return value; }
  }; // a compile time string holding a parameter pack.

  // who knows, it varies by compiler?
  //template <char...cs>
  template <typename T, T...cs>
  auto operator""_symbol() -> symbol<cs...> & {
    return symbol<cs...>::instance;
  }

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

  template <typename T, T X, T ... Ys>
  constexpr std::size_t find = detail::find_<0,T,X,Ys...>::value();

  namespace detail {
    template <char N, typename S>
    struct symbol_find_{};

    template <char N, char ... Cs>
    struct symbol_find_<N,symbol<Cs...>> {
      static constexpr std::size_t value = find<char,N,Cs...>;
    };
  }

  template <char N, typename S>
  constexpr std::size_t symbol_find = detail::symbol_find_<N,S>::value;

  // namespace detail {
  //   template <typename S> struct symbol_len_{

  //   };
  //   template <char...cs> struct symbol_len_<str<cs...>> {
  //     static constexpr size_t value = sizeof...(cs);
  //   };
  // };

  // template <typename S>
  // constexpr auto str_len = typename detail::symbol_len_<S>::value;

*/  

}
