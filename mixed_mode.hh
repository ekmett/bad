#pragma once
#include <type_traits>
#include "attributes.hh"
#include "sequences.hh"
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
  using namespace mixed_mode::common;
}

namespace bad::mixed_mode {
  using namespace sequences::api;
  template <class S>
  struct seq_for;

  template <class T, T... ts>
  struct seq_for<iseq<T,ts...>> {
    template <class F>
    BAD(hd,inline,flatten)
    seq_for(F f) {
       (f(ts),...,void());
    }
  };

  template <auto N>
  using forN = seq_for<make_seq<N>>;
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

    template <size_t i>
    BAD(hd,nodiscard,inline,flatten)
    auto dual() const noexcept {
      return me().template dual<i>();
    }

    /// compute partials w.r.t. expression arguments
    template <class T>
    BAD(hd,nodiscard,inline,flatten)
    auto partials(T bar) const noexcept {
      return me().partials(bar);
    }

    /// compute the product of partials and tangents
    template <class T, size_t j>
    BAD(hd,nodiscard,inline,flatten)
    auto tangent(T parts) const noexcept {
      return me().template tangent<j>(parts);
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

  template <class T, class Tangents = std::tuple<T>>
  class mixed;

  template <class T, class... Ds>
  class BAD(empty_bases,nodiscard) mixed<T,std::tuple<Ds...>>
  :                     mixed_expr<mixed<T,std::tuple<Ds...>>> {
    static constexpr size_t size = sizeof...(Ds);
    static constexpr size_t args = 1;

    using partials_type = T;
    using tangents = std::tuple<Ds...>;
    using element = T;

    T p;
    tangents d;

    BAD(hd,inline) constexpr
    mixed() noexcept : p(), d() {}

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
      using std::get;
      static_assert(std::is_same_v<tangents, typename B::tangents>);
      if constexpr (prefer_forward(B::args,size)) {
        forN<size> op([&](size_t i) {
          get<i>(d) = rhs.template dual<i>();
        });
      } else {
        auto parts = rhs.partials(1);
        forN<size> op([&](size_t i) {
          get<i>(d) = rhs.template tangent<i>(parts);
        });
      }
    }

    template <class B>
    BAD(hd,reinitializes,inline,flatten)
    mixed & operator =(mixed_expr<B> const & rhs) noexcept {
      using std::get;
      static_assert(std::is_same_v<tangents, typename B::tangents>);
      p = rhs.primal();
      if constexpr (prefer_forward(B::args,size)) {
        forN<size> op([&](size_t i) {
          get<i>(d) = rhs.template dual<i>();
        });
      } else {
        auto parts = rhs.partials(1);
        forN<size> op([&](size_t i) {
          get<i>(d) = rhs.template tangent<i>(parts);
        });
      }
      return *this;
    }

    template <class B>
    BAD(hd,inline,flatten)
    mixed & operator +=(mixed_expr<B> const & rhs) noexcept {
      using std::get;
      static_assert(std::is_same_v<tangents, typename B::tangents>);
      p += rhs.primal();
      if constexpr (prefer_forward(B::args,size)) {
        forN<size> op([&](size_t i) {
          get<i>(d) += rhs.template dual<i>();
        });
      } else {
        auto parts = rhs.partials(1);
        forN<size> op([&](size_t i) {
          get<i>(d) += rhs.template tangent<i>(parts);
        });
      }
      return *this;
    }

    template <class B>
    BAD(hd,inline,flatten)
    mixed & operator -=(mixed_expr<B> const & rhs) noexcept {
      using std::get;
      static_assert(std::is_same_v<tangents, typename B::tangents>);
      p -= rhs.primal();
      if constexpr (prefer_forward(B::args,size)) {
        forN<size> op([&](size_t i) {
          get<i>(d) -= rhs.template dual<i>();
        });
      } else {
        auto parts = rhs.partials(1);
        forN<size> op([&](size_t i) {
          get<i>(d) -= rhs.template tangent<i>(parts);
        });
      }
      return *this;
    }

    BAD(hd,nodiscard,inline) constexpr
    T primal() const noexcept {
      return p;
    }

    template <size_t i>
    BAD(hd,nodiscard,inline) constexpr
    T dual() const noexcept {
      using std::get;
      return get<i>(d);
    }

    BAD(hd,nodiscard,inline) constexpr
    T partials(T x) noexcept {
      return x;
    }

    template <size_t i>
    BAD(hd,nodiscard,inline) constexpr
    T tangent(T bar) const noexcept {
      using std::get;
      return bar * get<i>(d);
    }
  };

  // copy shape from a mixed expression
  template <class B>
  mixed(mixed_expr<B> const &) -> mixed<std::remove_reference_t<decltype(std::declval<B>().primal())>,typename B::tangents>;

  // copy shape from another mixed
  template <class T, class Tangents>
  mixed(mixed<T,Tangents> const &) -> mixed<T,Tangents>;

  /// simple lifted scalar
  /// \ingroup mixed_mode_group
  template <class T, typename Tangents>
  struct BAD(empty_bases,nodiscard) mixed_lift
  : mixed_expr<mixed_lift<T,Tangents>> {
    static constexpr size_t size = std::tuple_size_v<Tangents>;
    static constexpr size_t args = 0;

    using element_type = T;
    using tangents = Tangents;

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

    template <size_t i>
    BAD(hd,nodiscard,inline)
    T dual() const noexcept {
      return 0;
    }

    BAD(hd,nodiscard,inline)
    partials_type partials(BAD(maybe_unused) T x) const noexcept {
      return {};
    }

    template <size_t i>
    BAD(hd,nodiscard,inline)
    T tangent(BAD(maybe_unused) partials_type) const noexcept {
      return 0;
    };
  };

  /// \ingroup mixed_mode_group
  template <class L, class R>
  BAD(hd,nodiscard,inline)
  bool operator == (
    BAD(noescape) mixed_expr<L> const & l,
    BAD(noescape) mixed_expr<R> const & r
  ) {
    return l.primal() == r.primal();
  }

  /// \ingroup mixed_mode_group
  template <class L, class R>
  BAD(hd,nodiscard,inline)
  bool operator != (
    BAD(noescape) mixed_expr<L> const & l,
    BAD(noescape) mixed_expr<R> const & r
  ) {
    return l.primal() != r.primal();
  }

  /// \ingroup mixed_mode_group
  template <class L, class R>
  BAD(hd,nodiscard,inline)
  bool operator < (
    BAD(noescape) mixed_expr<L> const & l,
    BAD(noescape) mixed_expr<R> const & r
  ) {
    return l.primal() < r.primal();
  }

  /// \ingroup mixed_mode_group
  template <class L, class R>
  BAD(hd,nodiscard,inline)
  bool operator > (
    BAD(noescape) mixed_expr<L> const & l,
    BAD(noescape) mixed_expr<R> const & r
  ) {
    return l.primal() > r.primal();
  }

  /// \ingroup mixed_mode_group
  template <class L, class R>
  BAD(hd,nodiscard,inline)
  bool operator <= (
    BAD(noescape) mixed_expr<L> const & l,
    BAD(noescape) mixed_expr<R> const & r
  ) {
    return l.primal() <= r.primal();
  }

  /// \ingroup mixed_mode_group
  template <class L, class R>
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
  template <class T, class Tangents = std::tuple<T>>
  struct BAD(empty_bases,nodiscard) mixed_arg
  : mixed_expr<mixed_arg<T,Tangents>> {
    static constexpr size_t size = std::tuple_size_v<Tangents>;
    static constexpr size_t args = 1;

    using tangents = Tangents;
    using element_type = T;
    using partials_type = T;

    T p;
    size_t j;

    BAD(hd,nodiscard,inline)
    T primal() const noexcept {
      return p;
    }

    template <size_t i>
    BAD(hd,nodiscard,inline)
    T dual() const noexcept {
      return i == j ? 1 : 0;
    }

    BAD(hd,nodiscard,inline)
    T partials(T dz) const noexcept {
      return dz;
    }

    template <size_t i>
    BAD(hd,nodiscard,inline)
    T tangent(T dx) const noexcept {
      return i == j ? dx : 0;
    };
  };

  /// \ingroup mixed_mode_group
  template <class L, class R>
  struct BAD(empty_bases,nodiscard) mixed_add_expr
  : mixed_expr<mixed_add_expr<L,R>> {

    static constexpr size_t size = L::size;
    static constexpr size_t args = L::args + R::args;

    static_assert(std::is_same_v<typename L::tangents,typename R::tangents>,"tangent type mismatch");

    using tangents = typename L::tangents;
    using element_type = decltype(std::declval<L>().primal() + std::declval<R>().primal());
    using partials_type = std::tuple<typename L::partials_type, typename R::partials_type>;

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

    template <size_t i>
    BAD(hd,nodiscard,inline,flatten) constexpr
    auto dual() const noexcept {
      return lhs.template dual<i>() + rhs.template dual<i>();
    }

    BAD(hd,nodiscard,inline,flatten) constexpr
    partials_type partials(element_type x) const noexcept {
      return { lhs.partials(x), rhs.partials(x) };
    }

    template <size_t i>
    BAD(hd,nodiscard,inline,flatten) constexpr
    auto tangent(partials_type const & ps) const noexcept {
      using std::get;
      return lhs.template tangent<i>(get<0>(ps))
           + rhs.template tangent<i>(get<1>(ps));
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

    static_assert(std::is_same_v<L::tangents,R::tangents>,"tangent type mismatch");

    static_assert(L::size == R::size, "tangent size mismatch");

    static constexpr size_t size = L::size;
    static constexpr size_t args = L::args + R::args;

    using tangents = typename L::tangents;
    using element_type = decltype(std::declval<L>().primal() * std::declval<R>().primal());
    using partials_type = std::tuple<typename L::partials_type, typename R::partials_type>;

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

    template <size_t i>
    BAD(hd,nodiscard,inline,flatten) constexpr
    auto dual() const noexcept {
      return lhs.template dual<i>() * rhs.primal()
           + lhs.primal() * rhs.template dual<i>();
    }

    BAD(hd,nodiscard,inline,flatten) constexpr
    partials_type partials(element_type bar) const noexcept {
      return {
        lhs.partials(bar * rhs.primal()),
        rhs.partials(lhs.primal() * bar)
      };
    }

    template <size_t i>
    BAD(hd,nodiscard,inline,flatten) constexpr
    auto tangent(partials_type const & ps) const noexcept {
      using std::get;
      return lhs.template tangent<i>(get<0>(ps))
           + rhs.template tangent<i>(get<1>(ps));
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

  template <class S, class F, class... Args>
  struct diff_ {
    static_assert(no<S>,"diff: not a sequence");
  };

  template <size_t... is, class F, class... Args>
  struct BAD(empty_bases,nodiscard) diff_<seq<is...>, F, Args...> {
    BAD(hd,nodiscard,inline,flatten)
    auto operator ()(F f, Args... args) {
      using Tangents = std::tuple<Args...>;
      std::tuple<Args...> t(std::forward(args)...);
      mixed result = f((mixed_arg<decltype(std::get<is>(t)),Tangents>(std::get<is>(t)),is)...);
      return std::tuple(result.primal(), result.template dual<is>()...);
    }
  };


  /// \ingroup mixed_mode_group
  template <class F, class... Args>
  BAD(hd,nodiscard,inline,flatten) constexpr
  auto diff(F f, Args... args) {
    diff_<make_seq<sizeof...(args)>, Args...> op;
    return op(f,std::forward(args)...);
  }
}

/// \}
