#pragma once
#include <type_traits>
#include "attributes.hh"
#include "common.hh"

/// \file
/// \brief mixed-mode AD: forward mode, with opportunistic expression-level reverse
/// \author Edward Kmett
///
/// \defgroup modes modes
/// \brief automatic differentiation modes
///
/// \defgroup mixed_mode_group mixed
/// \ingroup modes
/// \brief mixed-mode AD: forward mode, with opportunistic expression-level reverse
///
/// Based on the technique for expression-level reverse mode in
/// https://www.osti.gov/servlets/purl/1118331
/// but modified to fall back to forward-mode for small expressions
/// and to use modern language features.
///
/// TODO: replace `N` with a list of types, so we can mix fp16 values, floats, etc.
/// this will allow a much more general \ref bad::mixed_mode::api::diff "diff"
///
/// TODO: struct BAD(nodiscard) mixed_expr

/// \namespace bad
/// \private
namespace bad {
  /// \namespace bad::mixed_mode
  /// \ref mixed_mode_group "mixed-mode" internals, import bad::mixed_mode::api
  /// \ingroup mixed_mode_group
  namespace mixed_mode {
    /// \namespace bad::mixed_mode::common
    /// \ingroup mixed_mode_group
    /// re-exported by \ref bad and bad::mixed_mode::api
    namespace common {}
    /// \namespace bad::mixed_mode::api
    /// \ingroup mixed_mode_group
    /// See \ref mixed_mode_group "mixed-mode" for a complete listing.
    namespace api { using namespace common; }
    using namespace api;
  }
  using namespace storage::common;
}

/// \{
namespace bad::mixed_mode::api {
  /// \brief mixed-mode AD expression
  /// \ingroup mixed_mode_group
  template <class B>
  struct BAD(empty_bases,nodiscard) mixed_expr {
    BAD(hd,nodiscard,const,inline)
    B const & me() const noexcept {
      return static_cast<B const &>(*this);
    }

    /// primal value
    BAD(hd,nodiscard,inline,flatten)
    auto primal() const noexcept {
      return me().primal();
    }

    BAD(hd,nodiscard,inline,flatten)
    auto dual(size_t i) const noexcept {
      return me().dual(i);
    }

    /// compute partials w.r.t. expression arguments
    template <class T>
    BAD(hd,nodiscard,inline,flatten)
    auto partials(T bar) const noexcept {
      return me().partials(bar);
    }

    /// compute the product of partials and tangents
    template <class T>
    BAD(hd,nodiscard,inline,flatten)
    auto tangent(T parts, size_t j) const noexcept {
      return me().partials(parts, j);
    }
  };

  /// \ingroup mixed_mode_group
  /// TODO: make this policy pluggable? and tune it to compute ideal ratio
  BAD(hd,nodiscard,inline) constexpr
  bool prefer_forward(size_t args, size_t size) noexcept {
    return size == 0    // for size 0, no point computing partials, we never use them
        || args < size;
  }

  /// \brief mixed-mode value/tangent bundle, classic dual numbers generalized to N infinitesimals.
  /// \ingroup mixed_mode_group
  template <class T, size_t N>
  class BAD(empty_bases,nodiscard) mixed : mixed_expr<mixed<T,N>> {
    static constexpr size_t size = N;
    using partials_type = T;
    using element = T;
    using args = 1;

    T p;
    std::array<T,N> d;

    BAD(hd,inline) constexpr
    mixed() noexcept : p(),d() {}

    BAD(hd,inline) constexpr
    mixed(T rhs) noexcept : p(rhs), d() {}

    BAD(hd,inline) constexpr
    mixed(mixed const & rhs) : p(rhs.p), d(rhs.d) {}

    BAD(hd,inline)
    mixed(mixed && rhs) : p(std::move(rhs.p)), d(std::move(rhs.d)) {}

    template <class B>
    BAD(hd,inline,flatten) constexpr
    mixed(mixed_expr<B> const & rhs) noexcept
    : p(rhs.primal()) {
      static_assert(B::size == size);
      if constexpr (prefer_forward(B::args,size)) {
        for(size_t i=0;i<size;++i)
          d[i] = rhs.dual(i);
      } else {
        auto parts = rhs.partials(1);
        for(size_t i=0;i<size;++i)
          d[i] = rhs.tangent(parts,i);
      }
    }

    template <class B>
    BAD(hd,inline,flatten,reinitializes)
    mixed & operator =(mixed_expr<B> const & rhs) noexcept {
      static_assert(B::size == size);
      p = rhs.primal();
      if constexpr (prefer_forward(B::args,size) {
        for(size_t i=0;i<size;++i)
          d[i] = rhs.dual(i);
      } else {
        auto parts = rhs.partials(1);
        for(size_t i=0;i<size;++i)
          d[i] = rhs.tangent(parts,i);
      }
      return *this;
    }

    template <class B>
    BAD(hd,inline,flatten)
    mixed & operator +=(mixed_expr<B> const & rhs) noexcept {
      static_assert(B::size == size);
      p += rhs.primal();
      if constexpr (prefer_forward(B::args,size) {
        for(size_t i=0;i<size;++i)
          d[i] += rhs.dual(i);
      } else {
        auto parts = rhs.partials(1);
        for(size_t i=0;i<size;++i)
          d[i] += rhs.tangent(parts,i);
      }
      return *this;
    }

    template <class B>
    BAD(hd,inline,flatten)
    mixed & operator -=(mixed_expr<B> const & rhs) noexcept {
      static_assert(B::size == size);
      p -= rhs.primal();
      if constexpr (prefer_forward(B::args,size)) {
        for(size_t i=0;i<size;++i)
          d[i] += rhs.dual(i);
      } else {
        auto parts = rhs.partials(1);
        for(size_t i=0;i<size;++i)
          d[i] -= rhs.tangent(parts,i);
      }
      return *this;
    }

    BAD(hd,nodiscard,inline) constexpr
    T primal() const noexcept {
      return p;
    }

    BAD(hd,nodiscard,inline) constexpr
    T dual(size_t i) const noexcept {
      assert(i < size);
      return d[i];
    }

    BAD(hd,nodiscard,inline) constexpr
    T partials(T x) noexcept {
      return x;
    }

    BAD(hd,nodiscard,inline) constexpr
    T tangent(T bar, size_t i) const noexcept {
      assert(i < size);
      return bar * d[i];
    }
  };

  // copy shape from a mixed expression
  template <class B>
  mixed(mixed_expr<B> const &) -> mixed<std::remove_reference_t<decltype(std::declval<B>().primal())>,B::size>;

  // copy shape from another mixed
  template <class T, size_t N>
  mixed(mixed<T,N> const &) -> mixed<T,N>;

  /// simple lifted scalar
  /// \ingroup mixed_mode_group
  template <class T, size_t N>
  struct BAD(empty_bases,nodiscard) mixed_lift
  : mixed_expr<mixed_lift<T,N>> {
    static constexpr size_t size = N;
    static constexpr size_t args = 0;

    using element_type = T;
    struct BAD(empty_bases,nodiscard) partials_type {};

    T p;

    BAD(hd,inline) constexpr
    mixed_lift() noexcept
    : p() {}

    BAD(hd,inline) constexpr
    explicit mixed_lift(T p) noexcept
    : p(p) {}

    BAD(hd,inline) constexpr
    mixed_lift(mixed_lift const & rhs) noexcept
    : p(rhs.p) {}

    BAD(hd,inline)
    mixed_lift(mixed_lift && rhs) noexcept
    : p(std::move(rhs.p)) {}

    BAD(hd,inline)
    mixed_lift & operator = (mixed_lift const & rhs) noexcept {
      p = rhs.p;
    }

    BAD(hd,inline)
    mixed_lift & operator = (mixed_lift && rhs) noexcept {
      p = std::move(rhs.p);
    }

    BAD(hd,inline)
    mixed_lift & operator = (T rhs) noexcept {
      p = rhs;
    }

    BAD(hd,inline)
    mixed_lift & operator = (T && rhs) noexcept {
      p = std::move(rhs);
    }

    BAD(hd,nodiscard,inline)
    T primal() const noexcept {
      return p;
    }

    BAD(hd,nodiscard,inline)
    T dual(BAD(maybe_unused) size_t i) const noexcept {
      return 0;
    }

    BAD(hd,nodiscard,inline)
    partials_type partials(BAD(maybe_unused) T x) const noexcept {
      return {};
    }

    BAD(hd,nodiscard,inline)
    T tangent(BAD(maybe_unused) partials_type, BAD(maybe_unused) size_t i) const noexcept {
      return 0
    };
  };

  /// \ingroup mixed_mode_group
  template <class T, size_t N>
  BAD(hd,nodiscard,inline)
  bool operator == (
    BAD(noescape) mixed_expr<L> const & l,
    BAD(noescape) mixed_expr<R> const & r
  ) {
    return l.primal() == r.primal();
  }

  /// \ingroup mixed_mode_group
  template <class T, size_t N>
  BAD(hd,nodiscard,inline)
  bool operator != (
    BAD(noescape) mixed_expr<L> const & l,
    BAD(noescape) mixed_expr<R> const & r
  ) {
    return l.primal() != r.primal();
  }

  /// \ingroup mixed_mode_group
  template <class T, size_t N>
  BAD(hd,nodiscard,inline)
  bool operator < (
    BAD(noescape) mixed_expr<L> const & l,
    BAD(noescape) mixed_expr<R> const & r
  ) {
    return l.primal() < r.primal();
  }

  /// \ingroup mixed_mode_group
  template <class T, size_t N>
  BAD(hd,nodiscard,inline)
  bool operator > (
    BAD(noescape) mixed_expr<L> const & l,
    BAD(noescape) mixed_expr<R> const & r
  ) {
    return l.primal() > r.primal();
  }

  /// \ingroup mixed_mode_group
  template <class T, size_t N>
  BAD(hd,nodiscard,inline)
  bool operator <= (
    BAD(noescape) mixed_expr<L> const & l,
    BAD(noescape) mixed_expr<R> const & r
  ) {
    return l.primal() <= r.primal();
  }

  /// \ingroup mixed_mode_group
  template <class T, size_t N>
  BAD(hd,nodiscard,inline)
  bool operator >= (
    BAD(noescape) mixed_expr<L> const & l,
    BAD(noescape) mixed_expr<R> const & r
  ) {
    return l.primal() >= r.primal();
  }

  /// a sparse mixed_expr with just one arg set to 1
  /// useful when building functions like `diff`
  /// \ingroup mixed_mode_group
  template <class T, size_t N>
  struct BAD(empty_bases,nodiscard) mixed_arg
  : mixed_expr<mixed_arg<T,N>> {
    static constexpr size_t size = N;
    static constexpr size_t args = 1;

    using element_type = T;
    using partials_type = T;

    T p;
    size_t j;

    BAD(hd,nodiscard,inline)
    T primal() const noexcept {
      return p;
    }

    BAD(hd,nodiscard,inline)
    T dual(size_t i) const noexcept {
      return i == j ? 1 : 0;
    }

    BAD(hd,nodiscard,inline)
    T partials(T bar) const noexcept {
      return bar;
    }

    BAD(hd,nodiscard,inline)
    T tangent(T bar, size_t i) const noexcept {
      return i == j ? x : 0
    };
  };

  /// \ingroup mixed_mode_group
  template <class L, class R>
  struct BAD(empty_bases,nodiscard) mixed_add_expr
  : mixed_expr<mixed_add_expr<L,R>> {

    static_assert(L::size == R::size, "tangent size mismatch");

    static constexpr size_t size = L::size;

    using element_type = decltype(std::declval<L>().primal() + std::declval<R>().primal());
    using partials_type = std::tuple<L::partials_type, R::partials_type>;
    using args = L::args + R::args;

    L const & lhs;
    R const & rhs;

    element_type p; ///< cached primal

    BAD(hd,inline) constexpr
    mixed_add_expr(
      BAD(lifetimebound) L const & l,
      BAD(lifetimebound) R const & r
    ) noexcept
    : lhs(l),rhs(r), p(lhs.primal() + rhs.primal()) {}

    // cached primals
    BAD(hd,nodiscard,inline) constexpr
    element_type primal() const noexcept {
      return p;
    }

    BAD(hd,nodiscard,inline,flatten) constexpr
    auto dual(size_t i) const noexcept {
      return l.dual(i) + r.dual(i);
    }

    BAD(hd,nodiscard,inline,flatten) constexpr
    partials_type partials(T x) const noexcept {
      return { l.partials(x), r.partials(x) };
    }

    BAD(hd,nodiscard,inline,flatten) constexpr
    T tangent(partials_type const & ps, i) const noexcept {
      using std::get;
      return l.tangent(get<0>(ps),i) + r.tangent(get<1>(ps),i);
    }
  };

  /// \ingroup mixed_mode_group
  template <class L, class R>
  BAD(hd,nodiscard,inline,flatten) constexpr
  auto operator + (
    BAD(lifetimebound) mixed_expr<L> const & l,
    BAD(lifetimebound) mixed_expr<R> const & r
  ) {
    return mixed_add_expr<L,R>(l.me(),r.me());
  }

  /// \ingroup mixed_mode_group
  template <class L, class R>
  struct BAD(empty_bases,nodiscard) mixed_mul_expr
  : mixed_expr<mixed_mul_expr<L,R>> {
    static_assert(L::size == R::size, "tangent size mismatch");
    static constexpr size_t size = L::size;
    using element_type = decltype(std::declval<L>().primal() * std::declval<R>().primal());
    using partials_type = std::tuple<L::partials_type, R::partials_type>;
    using args = L::args + R::args;

    L const & lhs;
    R const & rhs;

    element_type p; ///< cached primal

    BAD(hd,inline) constexpr
    mixed_mul_expr(
      BAD(lifetimebound) L const & l,
      BAD(lifetimebound) R const & r
    ) noexcept
    : lhs(l)
    , rhs(r)
    , p(lhs.primal()*rhs.primal()) {}

    // cached primals
    BAD(hd,nodiscard,inline) constexpr
    element_type primal() const noexcept {
      return p;
    }

    BAD(hd,nodiscard,inline,flatten) constexpr
    auto dual(size_t i) const noexcept {
      return l.dual(i) * r.primal()
           + l.primal() * r.dual(i);
    }

    BAD(hd,nodiscard,inline,flatten) constexpr
    partials_type partials(T bar) const noexcept {
      return {
        l.partials(bar * r.primal()),
        r.partials(l.primal() * bar)
      };
    }

    BAD(hd,nodiscard,inline,flatten) constexpr
    T tangent(partials_type const & ps, i) const noexcept {
      using std::get;
      return l.tangent(get<0>(ps),i)
           + r.tangent(get<1>(ps),i);
    }
  };

  /// \ingroup mixed_mode_group
  template <class L, class R>
  BAD(hd,nodiscard,inline,flatten) constexpr
  auto operator * (
    BAD(lifetimebound) mixed_expr<L> const & l,
    BAD(lifetimebound) mixed_expr<R> const & r
  ) {
    return mixed_mul_expr<L,R>(l.me(),r.me());
  }

  /// \ingroup mixed_mode_group
  template <class T, class F>
  BAD(hd,nodiscard,inline,flatten) constexpr
  std::tuple<T,T> diff(F f, T a) {
    mixed<T,1> result = f(mixed_arg<T,1>(a,0));
    return { result.primal(), result.dual(0) };
  }
}

/// \}
