#ifndef BAD_STORAGE_STORE_EXPR_HH
#define BAD_STORAGE_STORE_EXPR_HH

#include "bad/storage/store_expr_iterator.hh"

/// \file
/// \brief storage expression templates
/// \author Edward Kmett

namespace bad::storage {
  /// variadic expression template
  /// \ingroup storage_group
  template <class B, size_t d, size_t... ds>
  struct BAD(empty_bases,nodiscard) store_expr {
    using const_iterator = detail::const_store_expr_iterator<B,d>;
    using iterator = detail::store_expr_iterator<B,d>;
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

    BAD(hd,nodiscard,inline,const)
    constexpr // this lifetimebound
    B & operator() () noexcept {
      return at();
    }

    BAD(hd,nodiscard,inline,const)
    constexpr // this lifetimebound
    B const & operator() () const noexcept {
      return at();
    }

    template <class... ts>
    BAD(hd,nodiscard,inline,flatten)
    //constexpr // this lifetimebound
    auto operator() (ts... is) noexcept {
      return at(is...);
    }


    template <class... ts>
    BAD(hd,nodiscard,inline,flatten)
    constexpr // this lifetimebound
    auto operator() (ts... is) const noexcept {
      return at(is...);
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

  /// Sometimes we want expression templates to store values, sometimes we want to store references inside of our expression templates.
  ///
  /// This acts as a solution to the temporary capture problem whenever we need to build a combinator that builds temporaries
  /// safely. (It arises when working with einsum, or in the recursive cases for applying pull to a sum for instance.
  ///
  /// This looks at the type passed and if it is T&&, we store a value, otherwise we store a reference.
  ///
  /// In this way such combinators can pass in temporaries to the expression template and we should be able to capture them
  /// automatically, correctly and safely.
  /// \ingroup storage_group
  template <class T>
  using sub_expr = std::conditional_t<
    std::is_rvalue_reference_v<T>,
    std::decay_t<T>,
    std::decay_t<T> const &
  >;

  namespace detail {

    /// \ingroup storage_group
    template <class B, size_t d, size_t...ds>
    struct BAD(empty_bases,nodiscard) store_rep_expr final
    : store_expr<store_rep_expr<B,d,ds...>,d,ds...> {
      using base_type = std::decay_t<B>;
      using element = typename base_type::element;
      using dim = seq<d,ds...>;
      static constexpr size_t arity = 1 + sizeof...(ds);

      sub_expr<B> base;

      BAD(hd,nodiscard,inline,pure)
      auto operator[](BAD(maybe_unused) size_t i) const noexcept {
        return base;
      }

      template <size_t N>
      BAD(hd,nodiscard,inline,flatten) // this lifetimebound
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
      auto rep() const noexcept -> store_rep_expr<store_rep_expr<B,d,ds...>,N,d,ds...> {
        return { *this };
      }

      BAD(hd)
      friend std::ostream & operator<<(std::ostream & os, store_rep_expr const & rhs) {
        return os << "rep<" << d << ">(" << rhs << ")";
      }

      template <auto j, decltype(j) i, decltype(j)...is>
      BAD(hd,nodiscard,inline,flatten) // this lifetimebound
      auto tie(size_t k) {
        if constexpr (i == j) {
          return base.template tied<j,d,is...>(k);
        } else {
          return base.template tie<j,is...>(k).template rep<d>();
        }
      }

      template <auto j, size_t jd, decltype(j) i, decltype(j)...is>
      BAD(hd,nodiscard,inline,flatten) // this lifetimebound
      auto tied(size_t k) {
        if constexpr (i == j) {
          static_assert(d == jd, "tied: known dimension size mismatch");
          return base.template tied<j,jd,is...>(k);
        } else {
          return base.template tied<j,jd,is...>(k).template rep<d>();
        }
      }
    };

    /// \ingroup storage_group
    template <class L, class R, size_t d, size_t... ds>
    struct BAD(empty_bases,nodiscard) store_add_expr final
    : store_expr<store_add_expr<L,R,d,ds...>,d,ds...> {
      using dim = seq<d,ds...>;
      using left_type = std::decay_t<L>;
      using right_type = std::decay_t<R>;
      using element = decltype(std::declval<typename left_type::element>() + std::declval<typename right_type::element>());

      sub_expr<L> l;
      sub_expr<R> r;

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
  }

  /// \ingroup storage_group
  template <class L, class R, size_t d, size_t... ds>
  BAD(hd,nodiscard,inline,const)
  auto operator +(
    BAD(lifetimebound) store_expr<L,d,ds...> const &l,
    BAD(lifetimebound) store_expr<R,d,ds...> const &r
  ) noexcept -> detail::store_add_expr<L,R,d,ds...> {
    return { {}, l.at(), r.at() };
  }

  /// \ingroup storage_group
  template <class L, class R, size_t d, size_t... ds>
  BAD(hd,nodiscard,inline)
  auto operator +(
    BAD(noescape) store_expr<L,d,ds...> && l,
    BAD(lifetimebound) store_expr<R,d,ds...> const & r
  ) noexcept -> detail::store_add_expr<L&&,R,d,ds...> {
    return {{}, std::move(l.at()), r.at()};
  }

  /// \ingroup storage_group
  template <class L, class R, size_t d, size_t... ds>
  BAD(hd,nodiscard,inline)
  auto operator +(
    BAD(lifetimebound) store_expr<L,d,ds...> const & l,
    BAD(noescape) store_expr<R,d,ds...> && r
  ) noexcept -> detail::store_add_expr<L,R&&,d,ds...> {
    return {{}, l.at(), std::move(r.at())};
  }

  /// \ingroup storage_group
  template <class L, class R, size_t d, size_t... ds>
  BAD(hd,nodiscard,inline)
  auto operator +(
    BAD(noescape) store_expr<L,d,ds...> && l,
    BAD(noescape) store_expr<R,d,ds...> && r
  ) noexcept -> detail::store_add_expr<L&&,R&&,d,ds...> {
    return {{}, std::move(l.at()), std::move(r.at())};
  }

  // TODO: move to some kind of non-member rep construction mechanism so rep() can take rvalue references

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

  // TODO: move to some kind of non-member pull construction mechanism so pull() can take rvalue references

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

namespace bad {
  using namespace bad::storage;
}

#endif
