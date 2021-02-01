#pragma once
#include "storage_store_expr_iterator.hh"

/// \file
/// \brief storage expression templates 
/// \author Edward Kmett
///
/// \{

namespace bad::storage::api {
  /// variadic expression template
  /// \ingroup storage_group
  template <class B, size_t d, size_t... ds>
  struct BAD(empty_bases,nodiscard) store_expr {
    using const_iterator = const_store_expr_iterator<B,d>;
    using iterator = store_expr_iterator<B,d>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using dim = seq<d,ds...>;
    static constexpr size_t arity = 1 + sizeof...(ds);

    template <size_t i>
    static constexpr size_t nth_dim = nth<i,d,ds...>;

    template <class C = B>
    using expr = store_expr<C,d,ds...>;

    using actual_type = B;

    BAD(hd,nodiscard,inline,flatten) constexpr
    auto operator[](size_t i) noexcept {
      return at(i);
    }

    BAD(hd,nodiscard,inline,flatten) constexpr
    auto operator[](size_t i) const noexcept {
      return at(i);
    }

    BAD(hd,nodiscard,inline,const) constexpr
    B & at() noexcept {
      return static_cast<B &>(*this);
    }

    BAD(hd,nodiscard,inline,const) constexpr
    B const & at() const noexcept {
      return static_cast<B const &>(*this);
    }

    template <auto j, decltype(j)...is>
    BAD(hd,nodiscard,inline,const)
    auto tie(size_t k) {
      return at().template tie<j,is...>(k);
    }

    template <auto j, size_t jd, decltype(j)...is>
    BAD(hd,nodiscard,inline,const)
    auto tied(size_t k) {
      return at().template tied<j,jd,is...>(k);
    }

    template <class... ts>
    BAD(hd,nodiscard,inline,flatten) constexpr // this lifetimebound
    auto at(size_t i) noexcept {
      return at()[i];
    }

    template <class... ts>
    BAD(hd,nodiscard,inline,flatten) constexpr // this lifetimebound
    auto at(size_t i) const noexcept {
      return at()[i];
    }

    template <class... ts>
    BAD(hd,nodiscard,inline,flatten) constexpr // this lifetimebound
    auto at(size_t i, size_t j, ts... ks) noexcept {
      return at()[i](j, ks...);
    }

    template <class... ts>
    BAD(hd,nodiscard,inline,flatten) constexpr // this lifetimebound
    auto at(size_t i, size_t j, ts... ks) const noexcept {
      return at()[i](j, ks...);
    }

    BAD(hd,nodiscard,inline,const) constexpr // this lifetimebound
    B & operator()() noexcept {
      return at();
    }

    BAD(hd,nodiscard,inline,const) constexpr // this lifetimebound
    B const & operator()() const noexcept {
      return at();
    }

    template <class... ts>
    BAD(hd,nodiscard,inline,flatten) constexpr // this lifetimebound
    auto operator()(ts... is) noexcept {
      return at(is...);
    }


    template <class... ts>
    BAD(hd,nodiscard,inline,flatten) constexpr // this lifetimebound
    auto operator()(ts... is) const noexcept {
      return at(is...);
    }

    template <class C>
    BAD(hd,nodiscard,inline) constexpr
    bool operator==(expr<C> const & r) const noexcept {
      auto l = at();
      for (size_t i=0;i<d;++i) {
        if (l[i] != r[i]) return false;
      }
      return true;
    }

    template <class C>
    BAD(hd,nodiscard,inline) constexpr
    bool operator!=(expr<C> const & r) const noexcept {
      auto l = at();
      for (size_t i=0;i<d;++i) {
        if (l[i] != r[i]) return true;
      }
      return false;
    }

    BAD(hd)
    friend std::ostream operator <<(std::ostream &os, store_expr const & rhs) {
      return os << rhs.at();
    }

    BAD(hd,nodiscard,inline,const) constexpr
    iterator begin() noexcept {
      return iterator(&at(), 0);
    }

    BAD(hd,nodiscard,inline,const) constexpr
    iterator end() noexcept {
      return iterator(&at(), d);
    }

    BAD(hd,nodiscard,inline,const) constexpr
    reverse_iterator rbegin() noexcept {
      return reverse_iterator(iterator(&at(), d-1));
    }

    BAD(hd,nodiscard,inline,const) constexpr
    reverse_iterator rend() noexcept {
      return reverse_iterator(iterator(&at(), -1));
    }

    BAD(hd,nodiscard,inline,const) constexpr
    const_iterator begin() const noexcept {
      return const_iterator(&at(), 0);
    }

    BAD(hd,nodiscard,inline,const) constexpr
    const_iterator end() const noexcept {
      return const_iterator(&at(), d);
    }

    BAD(hd,nodiscard,inline,const) constexpr
    const_iterator cbegin() const noexcept {
      return const_iterator(&at(), 0);
    }

    BAD(hd,nodiscard,inline,const) constexpr
    const_iterator cend() const noexcept {
      return const_iterator(&at(), d);
    }

    BAD(hd,nodiscard,inline,const) constexpr
    const_reverse_iterator rbegin() const noexcept {
      return reverse_iterator(const_iterator(&at(), d-1));
    }

    BAD(hd,nodiscard,inline,const) constexpr
    const_reverse_iterator rend() const noexcept {
      return reverse_iterator(const_iterator(&at(), -1));
    }

    BAD(hd,nodiscard,inline,const) constexpr
    const_reverse_iterator crbegin() const noexcept {
      return reverse_iterator(const_iterator(&at(), d-1));
    }

    BAD(hd,nodiscard,inline,const) constexpr
    const_reverse_iterator crend() const noexcept {
      return reverse_iterator(const_iterator(&at(), -1));
    }
  };

  /// \ingroup storage_group
  template <class B, size_t d, size_t...ds>
  struct BAD(empty_bases,nodiscard) store_rep_expr : store_expr<store_rep_expr<B,d,ds...>,d,ds...> {
    using element = typename B::element;
    using dim = seq<d,ds...>;
    static constexpr size_t arity = 1 + sizeof...(ds);

    B const & base;

    BAD(hd,nodiscard,inline,pure)
    auto operator[](BAD(maybe_unused) size_t i) const noexcept {
      return base;
    }

    template <size_t N>
    BAD(hd,nodiscard,inline,flatten)
    auto pull(size_t i) const noexcept {
      if constexpr(N == 0) {
        return base;
      } else {
        return base.template pull<N-1>(i).template rep<d>();
      }
    }

    // this is lifetimebound, move out of line?
    template <size_t N>
    BAD(hd,nodiscard,inline,flatten,const)
    auto rep() const noexcept -> store_rep_expr<store_rep_expr,N,d,ds...> {
      return { *this };
    }

    BAD(hd)
    friend std::ostream & operator<<(std::ostream & os, store_rep_expr const & rhs) {
      return os << "rep<" << d << ">(" << rhs << ")";
    }

    template <auto j, decltype(j) i, decltype(j)...is>
    BAD(hd,nodiscard,inline,flatten)
    auto tie(size_t k) {
      if constexpr(i == j) {
        return base.template tied<j,d,is...>(k);
      } else {
        return base.template tie<j,is...>(k).template rep<d>();
      }
    }

    template <auto j, size_t jd, decltype(j) i, decltype(j)...is>
    BAD(hd,nodiscard,inline,flatten)
    auto tied(size_t k) {
      if constexpr(i == j) {
        static_assert(d == jd, "tied: known dimension size mismatch");
        return base.template tied<j,jd,is...>(k);
      } else {
        return base.template tied<j,jd,is...>(k).template rep<d>();
      }
    }
  };

  /// \ingroup storage_group
  template <class L, class R, size_t d, size_t... ds>
  struct BAD(empty_bases,nodiscard) store_add_expr
  : store_expr<store_add_expr<L,R,d,ds...>,d,ds...> {
    using dim = seq<d,ds...>;
    using element = decltype(std::declval<typename L::element>() + std::declval<typename R::element>());

    L const & l;
    R const & r;

    BAD(hd)
    store_add_expr(store_expr<L,d,ds...> const & l, store_expr<R,d,ds...> const & r) : l(l()), r(r()) {}

    BAD(hd,nodiscard,inline,pure)
    auto operator[](BAD(maybe_unused) size_t i) const noexcept {
      return l[i] + r[i];
    }

    template <size_t N>
    BAD(hd,nodiscard,inline,flatten)
    auto pull(size_t i) const noexcept {
      return l.template pull<N>(i) + r.template pull<N>(i);
    }

    template <auto j, decltype(j) i, decltype(j)...is>
    BAD(hd,nodiscard,inline,flatten)
    auto tie(size_t k) {
      return l.template tie<j,is...>(k) + r.template tie<j,is...>(k);
    }

    template <auto j, size_t jd, decltype(j) i, decltype(j)...is>
    BAD(hd,nodiscard,inline,flatten)
    auto tied(size_t k) {
      return l.template tied<j,jd,is...>(k) + r.template tied<j,jd,is...>(k);
    }

    template <size_t N>
    BAD(hd,nodiscard,inline,flatten,const)
    auto rep() const noexcept -> store_rep_expr<store_add_expr,N,d,ds...> {
      return { *this };
      // return l.template rep<N>(i) + r.template rep<N>(i);
    }

    // TODO: precedence pretty printing
    BAD(hd)
    friend std::ostream & operator<<(std::ostream & os, store_add_expr const & rhs) {
      return os << "(" << rhs.l << ") + (" << rhs.r << ")";
    }
  };

  /// \ingroup storage_group
  template <class L, class R, size_t d, size_t... ds>
  BAD(hd,nodiscard,inline,const)
  auto operator +(
    BAD(lifetimebound) store_expr<L,d,ds...> const &l,
    BAD(lifetimebound) store_expr<R,d,ds...> const &r
  ) noexcept {
    return store_add_expr<L,R,d,ds...>(l(),r());
  }
}

namespace bad::storage::common {
  /// \ingroup storage_group
  template <size_t d0, class B, size_t d1, size_t... ds>
  BAD(hd,nodiscard,inline)
  auto rep(BAD(lifetimebound) store_expr<B,d1,ds...> const & x) noexcept {
    return x().template rep<d0>();
  }

  /// \ingroup storage_group
  template <size_t d0, class B, size_t d1, size_t... ds>
  BAD(hd,nodiscard,inline)
  auto rep(BAD(lifetimebound) store_expr<B,d1,ds...> & x) noexcept {
    return x().template rep<d0>();
  }

  /// pull the `N`th dimension of a storage expression to the front.
  /// \ingroup storage_group
  template <size_t N, class B, size_t d, size_t... ds>
  BAD(hd,nodiscard,inline)
  auto pull(
    BAD(lifetimebound) store_expr<B,d,ds...> const & rhs,
    size_t i
  ) noexcept {
    return rhs().template pull<N>(i);
  }

  /// pulls the `N`th dimension of a store to the front.
  /// \ingroup storage_group
  template <size_t N, class B, size_t d, size_t... ds>
  BAD(hd,nodiscard,inline)
  auto pull(
    BAD(lifetimebound) store_expr<B,d,ds...> & rhs,
    size_t i
  ) noexcept {
    return rhs().template pull<N>(i);
  }
}

/// \}
