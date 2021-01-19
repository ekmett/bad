#pragma once
#include <string>
#include <cstdint>
namespace bad {
  // issue: this isn't enough to let me actually write einsum("ij,jk->kl")(x,y)
  // c++20 is needed for composite type template parameters
  // can't check dimensions dynamically. blah

  // template <symbol ... xs> struct symbols {};
  // when c++20 finally makes std::string constexpr, we can move to that.
  class symbol {
  private:
    const char * const p;
    const std::size_t s;
  public:

    static constexpr std::size_t npos = std::string::npos;
    using iterator = char const *;
    using const_iterator = char const *;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<iterator>;

    template <std::size_t N> 
    constexpr symbol(const char(&a)[N]) noexcept : p(a), s(N-1) {}

    constexpr explicit symbol(const char * p, std::size_t s) : p(p), s(s) {}

    constexpr char at(std::size_t n) const {
      return n < s ? p[n] : throw std::out_of_range("");
    }

    constexpr char operator[](std::size_t n) const {
      return n < s ? p[n] : throw std::out_of_range("");
    }

    constexpr bool empty() const noexcept { return s == 0; }

    constexpr std::size_t size() const noexcept { return s; } 

    //constexpr in c++20;
    operator std::string () const noexcept { return std::string(p, p + s); }
    constexpr symbol take(std::size_t n) const noexcept { 
      return symbol(p,std::min(n,s)); 
    }
    constexpr symbol drop(std::size_t n) const noexcept { 
      return symbol(p+std::min<std::size_t>(n,s), std::max<std::size_t>(s-n,0)); 
    }
    constexpr bool operator == (const symbol & rhs) const noexcept {
      if (size() != rhs.size()) return false;
      for (int i=0;i<s;++i)
        if (at(i) != rhs.at(i)) return false;
      return true;
    }
    constexpr bool operator != (const symbol & rhs) const noexcept {
      if (size() != rhs.size()) return true;
      for (int i=0;i<s;++i)
        if (at(i) != rhs.at(i)) return true;
      return false;
    }
    constexpr const_iterator begin() const noexcept { return p; }
    constexpr const_iterator end() const noexcept { return p + s; }
    constexpr const_iterator cbegin() const noexcept { return p; }
    constexpr const_iterator cend() const noexcept { return p + s; }
    constexpr const_reverse_iterator rbegin() const noexcept { return std::reverse_iterator(p + s - 1); }
    constexpr const_reverse_iterator rend() const noexcept { return std::reverse_iterator(p - 1); }
    constexpr const_reverse_iterator crbegin() const noexcept { return std::reverse_iterator(p + s - 1); }
    constexpr const_reverse_iterator crend() const noexcept { return std::reverse_iterator(p - 1); }

    constexpr std::size_t count(char c) const noexcept {
      std::size_t n = 0;
      for (std::size_t i=0;i<s;++i)
        if (at(i) == c) ++n;
      return n;
    }

    constexpr std::size_t count(symbol c) const noexcept {
      std::size_t n = 0;
      for (std::size_t i=0;i<s;++i)
        if (drop(i).take(c.size()) == c) ++n;
      return n;
    }

    constexpr bool contains(char c) const noexcept {
      for (std::size_t i=0;i<s;++i)
        if (at(i) == c) return true;
      return false;
    }

    constexpr bool contains(symbol c) const noexcept {
      for (std::size_t i=0;i<s;++i)
        if (drop(i).take(c.size()) == c) return true;
      return false;
    }

    constexpr std::size_t index(char c, std::size_t pos = 0) const noexcept {
      for (;pos<s;++pos)
        if (at(pos) == c) return pos;
      return npos;
    }

    constexpr std::size_t index(symbol c, std::size_t pos = 0) const noexcept {
      for (;pos<s;++pos)
        if (drop(pos).take(c.size()) == c) return pos;
      return npos;
    }
  };


}
