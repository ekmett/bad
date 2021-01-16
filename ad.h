#pragma once
#include <tuple>
#include <cstdint>
#include <limits>
#include <vector>

// debug
#include <dlfcn.h>
#include <iostream>

namespace autodiff {
  template <typename T, typename Act> struct tape;
  using index_t = std::uint_fast32_t;

  namespace detail {
    template <typename T, typename Act> struct ad_types {
      using element_t = T;
      using activation_t = Act;
      using tape_t = tape<T,Act>;
    };

    template <typename T, typename Act>
    using node = void (*)(T const * & e, index_t const * & i, Act act, index_t & base);
  };

  // default activations are for single results
  template <typename T, typename Act = T*>
  struct tape : detail::ad_types<T,Act> {
    std::vector<T> data;
    std::vector<index_t> indices;
    std::vector<detail::node<T,Act>> entries;
    index_t head; // scalar activation records required

    tape(int head = 0) : data(), indices(), entries(), head(head) {}

    bool operator==(const tape &rhs) const { return this = &rhs; }

    void propagate(Act a) {
      index_t b = head;
      T const * d = data.data()+data.size(); // .cend();
      index_t const * i = indices.data() + indices.size(); // indices.cend();
      for (auto e = entries.rbegin(); e != entries.rend(); ++e)
        (*e)(d,i,a,b);
    }

    index_t advance(int i = 1) {
      return head += i;
    }
  };

/*
  index_t index(auto...indices) {
    index_t idx = 0, i = 0;
    (..., (idx += indices * strides[i++]));
    return idx;
  }
*/

  // example using this for simple scalar operations
  namespace detail {

    template <typename F, typename ... Args> void foreach(F f, Args ... args) {
      (f(args), ...);
    }

    // c++17 fold expression
    template <typename T, typename Act, T ... Ks>
    void static_linear(T const * & e, index_t const * & i, Act a, index_t & b) {
      index_t lb = --b;
      constexpr index_t di = sizeof...(Ks);
      index_t const * j = i -= di;
      constexpr T ts[di] = { Ks ... };
      for (const T & t : ts)
        a[*j++] += t * a[lb];
    }

    template <typename T, typename Act, int N>
    void dynamic_linear(T const * & e, index_t const * & i, Act a, index_t & b) {
      index_t lb = --b;
      index_t const * j = i -= N;
      const T * f = e -= N;
      for (index_t k = 0;k<N;++k)
        a[j[k]] += f[k] * a[lb];
    }
  };

  constexpr index_t no_index = std::numeric_limits<index_t>::max();

  // ad<T> -- uses the tape and scalar operations
  template <typename T, typename Act = T*> // , typename TapeRef = tape<T,Act> &>
  struct ad {

    using tape_t = tape<T, Act>;

    mutable tape_t * tape; // TapeRef tape;
    index_t index;

    T primal;

    template <typename ... Args>
    void push_index(Args ... is) const {
      (tape->indices.emplace_back(is),...);
    }

    template <typename ... Args>
    void push_entry(Args ... is) const {
      (tape->entries.emplace_back(is),...);
    }

    template <typename ... Args>
    void push_data(Args ... is) const {
      (tape->data.emplace_back(is),...);
    }

    index_t advance(int i = 1) const {
      return tape->advance(i);
    }

    // lifting constants
    ad() noexcept // noexcept(noexcept(T()) && noexcept(TapeRef()))
    : primal()
    , index(no_index)
    , tape() {}

    ad(ad && rhs) noexcept // noexcept(noexcept(T(std::move_if_noexcept(rhs.primal))) && noexcept(TapeRef(std::move_if_noexcept(rhs.tape))))
    : primal(std::move_if_noexcept(rhs.primal))
    , index(std::move(rhs.index))
    , tape(std::move_if_noexcept(rhs.tape)) {}

    ad(const ad & rhs) noexcept // noexcept(noexcept(T(rhs.primal)) && noexcept(TapeRef(rhs.tape))
    : primal(rhs.primal)
    , index(rhs.index)
    , tape(rhs.tape) {}

    // lift
    ad(const T & primal) noexcept : primal(primal), tape(), index(no_index) {}

    // evil
    ad(const T & primal, tape_t * tape, index_t index) noexcept : primal(primal), tape(tape), index(index) {}

    bool is_const() const noexcept { return index == no_index; }

    ad operator * (const T & rhs) const noexcept {
      auto p = primal * rhs;
      if (is_const()) return ad(p);
      push_index(index);
      push_data(rhs);
      push_entry(&detail::dynamic_linear<T,Act,1>);
      return ad(p, tape, advance());
    }

    ad operator * (const ad & rhs) const noexcept {
      auto p = primal * rhs.primal;
      if (is_const()) {
        if (rhs.is_const()) return ad(p);
        // constant * non-constant
        rhs.push_index(rhs.index);
        rhs.push_data(primal);
        rhs.push_entry(&detail::dynamic_linear<T,Act,1>);
        return ad(p, rhs.tape, rhs.advance());
      }

      if (rhs.is_const()) {
        // non-constant * constant
        push_index(index);
        push_data(rhs.primal);
        push_entry(&detail::dynamic_linear<T,Act,1>);
        return ad(p, tape, advance());
      }

      // non-constant * non-constant
      assert(tape == rhs.tape);
      push_index(index, rhs.index);
      push_data(rhs.primal, primal);
      push_entry(&detail::dynamic_linear<T,Act,2>);
      return ad(p, tape, advance());
    }

    const ad & operator *= (const T & rhs) const noexcept {
      primal *= rhs;
      if (!is_const()) {
        push_index(index);
        push_data(rhs.primal);
        push_entry(&detail::dynamic_linear<T,Act,1>);
        index = advance();
      }
      return *this;
    }

    ad operator + (const T & rhs) const noexcept { return ad(primal + rhs, tape, index); }
    ad operator - (const T & rhs) const noexcept { return ad(primal - rhs, tape, index); }

    const ad & operator += (const T & rhs) noexcept { primal += rhs; return *this; }
    const ad & operator -= (const T & rhs) noexcept { primal -= rhs; return *this; }

    ad operator + (const ad & rhs) const noexcept {
      auto p = primal + rhs.primal;
      if (is_const()) return ad(p, rhs.tape, rhs.index);
      if (rhs.is_const()) return ad(p, tape, index);
      push_index(index, rhs.index);
      push_entry(&detail::static_linear<T,Act,1,1>);
      return ad(p, tape, advance());
    }

    const ad & operator -= (const ad & rhs) noexcept {
      primal -= rhs.primal;
      if (!rhs.is_const())  {
        if (is_const()) {
          tape = rhs.tape; // we had a bogus tape, copy it if needed.
          push_entry(&detail::static_linear<T,Act,-1>);
        } else {
          assert(tape == rhs.tape);
          push_entry(&detail::static_linear<T,Act,1,-1>);
          push_index(index);
        }
        push_index(rhs.index);
        index = advance();
      }
      return *this;
    }

    const ad & operator - (const ad & rhs) noexcept {
      auto p = primal - rhs.primal;
      if (rhs.is_const()) return ad(p, tape, index);
      if (is_const()) {
        rhs.push_index(index);
        rhs.push_entry(&detail::static_linear<T,Act,-1>);
        return ad(p, rhs.tape, rhs.tape.advance());
      }
      // non-constant - non-constant
      assert(tape == rhs.tape);
      push_index(index, rhs.index);
      push_entry(&detail::static_linear<T,Act,1,-1>);
      return ad(p, tape, advance());
    }
  }; // ad

  // left hand scalar multiplication
  template <typename T, typename Act> ad<T,Act> operator + (const T & lhs, const ad<T,Act> & rhs) {
    return ad(lhs + rhs.primal, rhs.tape, rhs.index);
  }

  template <typename T, typename Act> ad<T,Act> operator - (const T & lhs, const ad<T,Act> & rhs) {
    auto p = lhs - rhs.primal;
    if (rhs.is_const()) return p;
    rhs.push_index(rhs.index);
    rhs.push_entry(&detail::static_linear<T,Act,-1>);
    return ad(p, rhs.tape, rhs.advance());
  }

  template <typename T, typename Act> ad<T,Act> operator * (const T & lhs, const ad<T,Act> & rhs) {
    auto p = lhs * rhs.primal;
    if (rhs.is_const()) return p;
    rhs.push_index(rhs.index);
    rhs.push_data(lhs);
    rhs.push_entry(&detail::dynamic_linear<T,Act,1>);
    return ad(p, rhs.tape, rhs.advance());
  }


  template<typename T>
  std::ostream &operator <<(std::ostream &os, const std::vector<T> &v) {
    using namespace std;
    copy(v.begin(), v.end(), ostream_iterator<T>(os, "\n"));
    return os;
  }

}

template<typename ... Args>
std::ostream &operator <<(std::ostream &os, void (*n)(Args...)) {
  return os << "WTFBBQ";
  Dl_info info;
  if (dladdr(n, &info)) {
    os << reinterpret_cast<intptr_t>(n) << " = " << info.dli_fname << ":" << info.dli_sname;
    return os;
  } else {
    os << reinterpret_cast<intptr_t>(n);
    return os;
  }
}

namespace autodiff {
  template <typename T, typename F>
  std::tuple<T,T> diff(F f, T a) {
    tape<T> t(1); // one entry
    ad<T> result = f(ad<T>(a,&t,0));

    std::cout << "data: " << t.data.size() << std::endl << t.data << std::endl;
    std::cout << "entries: " << t.entries.size() << std::endl << t.entries << std::endl;
    std::cout << "indices: " << t.indices.size() << std::endl << t.indices << std::endl;
    std::cout << "head: " << t.head << std::endl;

    std::unique_ptr<T[]> activations (new T[t.head]);
    activations[t.head-1] = 1;
    t.propagate(activations.get());
    return { result.primal, activations[0] };
  }
} // namespace autodiff
