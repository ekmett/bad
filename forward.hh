#pragma once

/// @file forward_mode.hh
/// @brief scalar forward mode
/// @author Edward Kmett

/// @defgroup forward forward
/// @brief forward mode
/// @{

namespace bad {
  /// forward mode
  namespace forward_mode {
    /// re-exported by \ref bad and \ref bad::forward::api
    namespace common {}
    /// public components
    namespace api { using namespace common; }
    using namespace api; 
  }
  using namespace bad::forward::common;
}

namespace bad::forward_mode::common {
  template <class B, size_t N = 1>
  struct forward_expr {
    BAD(hd,inline) // this is lifetimebound
    B const & me() const noexcept {
      return static_cast<B const &>(*this);
    }

    BAD(hd,inline,flatten)
    auto primal() const noexcept {
      return me().primal();
    }

    BAD(hd,inline,flatten)
    auto dual(size_t i) const noexcept {
      return me().dual(i);
    }
  };

/*
  // over-engineered variadic sum
  template <size_t N, class...Ts>
  struct forward_add_expr
  : forward_expr<forward_add_expr<N,Ts...>,N> {
    using element = decltype(std::declval<Ts const>().primal() + ... + 0);
    std::tuple<Ts const & ...> const & t;

    BAD(hd,inline,flatten)
    primal(forward_expr<Ts,N> const & ... is) noexcept
    : t(make_tuple(is.me()...)) {}

    BAD(hd,inline,flatten)
    element primal() const noexcept {
      return std::apply([](auto... v) { return (v.primal() + ... + 0); }, t);
    }

    BAD(hd,inline,flatten)
    element dual(size_t i) const noexcept {
      return std::apply([=i](auto... v) { return (v.dual(i) + ... + 0); }, t)
    }
  };
*/


  /// when dF doesn't depend on the primals, e.g. addition, constant linear multiples, etc.
  // F is fed all the arguments variadically
  template <size_t N, class F, class dF, class... Bs> 
  struct forward_simple_expr
  : forward_expr<forward_simple_expr<N,F,dF,Bs...>,N> {
    F f;
    dF df;
    std::tuple<Bs const &...> ts;
    using element = decltype(f(std::declval<Bs::element>()...));

    BAD(hd,inline)
    forward_simple_expr(F f, dF df, forward_expr<Bs,N> const & ... ts)
    : f(f), df(df), ts(std::make_tuple(ts.me()...)) {}
    
    BAD(hd,inline,flatten)
    element primal() const noexcept {
      return std::apply([&](auto... v) { return f(v.primal()...); },t);
    }

    BAD(hd,inline,flatten)
    element dual(size_t i) const noexcept {
      return std::apply([&](auto... v) { return df(v.dual(i)...); },t);
    }
  };

  template <size_t N, class A, class B>
  BAD(hd,inline,const)
  auto operator + (
    BAD(lifetimebound) forward_expr<A,N> const & lhs
    BAD(lifetimebound) forward_expr<B,N> const & rhs
  ) { 
    auto f = [](typename A::element x, typename B::element y) { return x + y; };
    // can inference save me the elements and decltypes?
    return forward_simple_expr<N,decltype(f),decltype(f),A,B>(f,f,lhs,rhs);
  }

  /// when dF doesn't depend on the primals, e.g. addition, constant linear multiples, etc.
  template <size_t N, class F, class dF, class... Bs> 
  struct forward_complex_expr
  : forward_expr<forward_complex_expr<N,F,dF,Bs...>,N> {
    F f;
    dF df;
    std::tuple<Bs const &...> ts;

    using element = decltype(f(std::declval<Bs::element>()...));

    std::tuple<Bs::element...> ps;

    BAD(hd,inline)
    forward_sighted_expr(F f, dF df, forward_expr<Bs,N> const & ... ts)
    : f(f), df(df), ts(std::make_tuple(ts.me()...))
    , ps(std::apply([](auto... v) { return std::make_tuple(v.primal()...); },ts)) {}
    
    BAD(hd,inline,flatten)
    element primal() const noexcept {
      return f(ps);
    }

    BAD(hd,inline,flatten)
    element dual(size_t i) const noexcept {
      return df(ps,ds);
    }
  }

  template <size_t N, class T, class B>
  auto operator * (T lhs, forward_expr<B,N> const & rhs) noexcept {
    return forward_sv_expr<N,T,B>(lhs,rhs);
  }

  template <size_t N, class T, class U> 
  struct forward_vs_expr
  : forward_expr<forward_vs_expr<N,T,U>,N> {

    using element = decltype(std::declval<U const>().primal() * std::declval<T>());

    U const & l;
    T r;
    decltype(l.primal() * r) p;

    BAD(hd,inline)
    forward_vs_expr(forward_expr<U,N> const & l, T r)
    : l(l.me()), r(r), p(l.primal() * r) {}
    
    BAD(hd,inline,flatten)
    element primal() const noexcept {
      return p;
    }

    BAD(hd,inline,flatten)
    element dual(size_t i) const noexcept {
      return l.dual(i) * r;
    }
  };

  template <size_t N, class T, class B>
  auto operator * (forward_expr<B,N> const & lhs, T rhs) noexcept {
    return forward_vs_expr<N,T,B>(lhs,rhs);
  }

  template <size_t N, class A, class B> 
  struct forward_vv_expr
  : forward_expr<forward_vs_expr<N,T,U>,N> {
    A const & l;
    B const & r;

    using element = decltype(l.primal() * r.primal());
    decltype(std::declvaldecltype<A>()element lp;
    element rp;

    BAD(hd,inline)
    forward_vs_expr(forward_expr<U,N> const & l, T r)
    : l(l.me()), r(r.me()), lp(l.primal()), rp(r.primal()) {}
    
    BAD(hd,inline,flatten)
    element primal() const noexcept {
      return p;
    }

    BAD(hd,inline,flatten)
    element dual(size_t i) const noexcept {
      return l * r.dual(i);
    }
  };

  template <size_t N, class A, class B>
  auto operator * (forward_expr<A,N> const & lhs, forward_expr<B,N> const & rhs) noexcept {
    return forward_vv_expr<N,A,B>(lhs,rhs);
  }
}

namespace bad::forward_mode::common {
  template <class T, size_t N = 1>
  struct forward : forward_expr<forward<T,N>,N> {
    using element = T;

    T p;
    std::array<T,N> d;

    BAD(hd,inline)
    forward() noexcept
    : p(), d() {}

    BAD(hd,inline)
    explicit forward(T p) noexcept
    : p(p), d() {}

    BAD(hd,inline)
    forward(forward const & rhs) noexcept
    : p(rhs.p), d(rhs.d) {}

    BAD(hd,inline)
    forward(forward && rhs) noexcept
    : p(std::move(rhs.p)), d(std::move(rhs.d)) {}

    template <class B>
    BAD(hd,inline,flatten)
    forward(forward_expr<B,N> const & rhs) {
      p = rhs.primal();
      for (int i=0;i<N;++i)
        d[i] = rhs.dual(i);
    }

    BAD(hd,inline)
    T primal() const noexcept {
      return p;
    }

    BAD(hd,inline)
    T dual(size_t i) const noexcept {
      return d[i];
    }

    forward & operator += (T rhs) noexcept {
      primal += rhs;
      return this;
    }


    template <class B>
    BAD(hd,inline,flatten)
    forward & operator += (forward_expr<B,N> const & rhs) noexcept {
      primal += rhs.primal();
      for (int i=0;i<N;++i)
        d[i] += rhs.dual(i);
      return *this;
    }

    template <class B>
    BAD(hd,inline,flatten)
    forward & operator -= (forward_expr<B,N> const & rhs) noexcept {
      primal += rhs.primal();
      for (int i=0;i<N;++i)
        d[i] -= rhs.dual(i);
      return *this;
    }
  };

  template <class T, size N>
  BAD(hd,inline,flatten)
  void swap(forward<T,N> & lhs, forward<T,N> & rhs) {
    using std::swap;
    swap(lhs.p,rhs.p);
    swap(lhs.d,rhs.d);
  }

  // CTAD copy forward input shape, and infer element type
  template <class B, size_t N>
  forward(forward_expr<B,N> const &) -> forward<B::value_type,N>;
}

/// @}
