#pragma once
#include <array>
#include "seq.hh"
#include "attrib.hh"
#include "store_expr.hh"

namespace bad {
  template <class B, size_t d, size_t... ds>
  struct store_expr {
    using dim = seq<d,ds...>;
    static constexpr size_t arity = 1 + sizeof...(ds);

    template <size_t i>
    static constexpr size_t nth_dim = nth<i,d,ds...>;

    template <class C = B>
    using expr = store_expr<C,d,ds...>;

    using actual_type = B;

    BAD(hd,inline,flatten)
    auto operator[](size_t i) noexcept {
      return at(i);
    }

    BAD(hd,inline,flatten)
    auto operator[](size_t i) const noexcept {
      return at(i);
    }

    BAD(hd,inline,const)
    B & at() noexcept {
      return static_cast<B &>(*this);
    }

    BAD(hd,inline,const)
    B const & at() const noexcept {
      return static_cast<B const &>(*this);
    }

    template <class... ts>
    BAD(hd,inline,flatten) // this lifetimebound
    auto at(size_t i) noexcept {
      return at()[i];
    }

    template <class... ts>
    BAD(hd,inline,flatten) // this lifetimebound
    auto at(size_t i) const noexcept {
      return at()[i];
    }

    template <class... ts>
    BAD(hd,inline,flatten) // this lifetimebound
    auto at(size_t i, size_t j, ts... ks) noexcept {
      return at()[i](j, ks...);
    }

    template <class... ts>
    BAD(hd,inline,flatten) // this lifetimebound
    auto at(size_t i, size_t j, ts... ks) const noexcept {
      return at()[i](j, ks...);
    }

    BAD(hd,inline,const) // this lifetimebound
    B & operator()() noexcept {
      return at();
    }

    BAD(hd,inline,const) // this lifetimebound
    B const & operator()() const noexcept {
      return at();
    }

    template <class... ts>
    BAD(hd,inline,flatten) // this lifetimebound
    auto operator()(ts... is) noexcept {
      return at(is...);
    }


    template <class... ts>
    BAD(hd,inline,flatten) // this lifetimebound
    auto operator()(ts... is) const noexcept {
      return at(is...);
    }

    template <class C>
    BAD(hd,inline) // constexpr?
    bool operator==(expr<C> & r) const noexcept {
      auto l = at();
      for (size_t i=0;i<d;++i) {
        if (l[i] != r[i]) return false;
      }
      return true;
    }

    template <class C>
    BAD(hd,inline) // constexpr
    bool operator!=(expr<C> const & r) noexcept {
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

    struct const_iterator {
      B const * p;
      ptrdiff_t i;

      using iterator_category = std::random_access_iterator_tag;
      using value_type        = std::add_const<std::remove_reference<decltype(p->at(0))>>;
      using difference_type   = ptrdiff_t;
      using reference_type    = std::add_lvalue_reference<value_type>;
      using pointer_type      = std::add_pointer<value_type>;

      BAD(reinitializes,hd,inline,noalias)
      const_iterator & operator =(const_iterator rhs) noexcept {
        p = rhs.p;
        i = rhs.i;
        return *this;
      }

      BAD(reinitializes,hd,inline,noalias)
      const_iterator & operator =(const_iterator && rhs) noexcept {
        p = std::move(rhs.p);
        i = std::move(rhs.i);
        return *this;
      }

      // NB: iterators are only comparable if they come from the same container
      BAD(hd,inline,pure)
      friend bool operator ==(const_iterator lhs, const_iterator rhs) noexcept {
        assert(lhs.p == rhs.p);
        return lhs.i == rhs.i;
      }

      // NB: iterators are only comparable if they come from the same container
      BAD(hd,inline,pure)
      friend bool operator !=(const_iterator lhs, const_iterator rhs) noexcept {
        assert(lhs.p == rhs.p);
        return lhs.i != rhs.i;
      }

      BAD(hd,inline,pure)
      friend bool operator <(const_iterator lhs, const_iterator rhs) noexcept {
        assert(lhs.p == rhs.p);
        return lhs.i < rhs.i;
      }

      BAD(hd,inline,pure)
      friend bool operator >(const_iterator lhs, const_iterator rhs) noexcept {
        assert(lhs.p == rhs.p);
        return lhs.i > rhs.i;
      }

      BAD(hd,inline,pure)
      friend bool operator <=(const_iterator lhs, const_iterator rhs) noexcept {
        assert(lhs.p == rhs.p);
        return lhs.i <= rhs.i;
      }

      BAD(hd,inline,pure)
      friend bool operator >=(const_iterator lhs, const_iterator rhs) noexcept {
        assert(lhs.p == rhs.p);
        return lhs.i >= rhs.i;
      }

      BAD(hd,inline,noalias)
      const_iterator & operator ++() noexcept {
        ++i;
        return *this;
      }

      BAD(hd,inline,noalias)
      const_iterator operator ++(int) noexcept {
        return { p, i++ };
      }

      BAD(hd,inline,noalias)
      const_iterator & operator --() noexcept {
        --i;
        return *this;
      }

      BAD(hd,inline,noalias)
      const_iterator operator --(int) noexcept {
        return { p, i-- };
      }

      BAD(hd,inline,pure)
      friend const_iterator operator +(const_iterator lhs, ptrdiff_t rhs) noexcept {
        return { lhs.p, lhs.i + rhs };
      }

      friend const_iterator operator +(ptrdiff_t lhs, const_iterator & rhs) noexcept {
        return { rhs.p, rhs.i + lhs };
      }

      BAD(hd,inline,pure)
      friend const_iterator operator -(const_iterator lhs, ptrdiff_t rhs) noexcept {
        return { lhs.p, lhs.i - rhs };
      }

      BAD(hd,inline,pure)
      friend ptrdiff_t operator -(const_iterator lhs, const_iterator rhs) noexcept {
        assert(lhs.p == rhs.p);
        return lhs.i - rhs.i;
      }

      BAD(hd,inline,noalias)
      const_iterator & operator +=(ptrdiff_t rhs) const noexcept {
        i += rhs;
        return *this;
      }

      BAD(hd,inline,noalias)
      const_iterator & operator -=(ptrdiff_t rhs) const noexcept {
        i -= rhs;
        return *this;
      }

      BAD(hd,inline,pure)
      auto & operator *() const noexcept {
        assert(valid());
        return p->at(i);
      }

      BAD(hd,inline,pure)
      auto * operator ->() const noexcept {
        assert(valid());
        return &(p->at(i));
      }

      BAD(hd,inline,pure)
      auto & operator[](difference_type di) const noexcept {
        assert(p && 0 <= i + di && i + di < d);
        return p->at(i + di);
      }

      BAD(hd,inline,pure)
      bool valid() const noexcept {
        return p != nullptr && 0 <= i && i < d;
      }
    };

    struct iterator {
      B * p;
      ptrdiff_t i;

      using iterator_category = std::random_access_iterator_tag;
      using value_type        = std::remove_reference<decltype(p->at(0))>;
      using difference_type   = ptrdiff_t;
      using reference         = std::add_lvalue_reference<value_type>;
      using pointer           = std::add_pointer<value_type>;

      BAD(hd,inline,noalias) constexpr
      iterator(const iterator & rhs) noexcept
      : p(rhs.p), i(rhs.i) {}

      BAD(hd,inline,noalias) constexpr
      iterator(iterator && rhs) noexcept
      : p(std::move(rhs.p)), i(std::move(rhs.i)) {}

      BAD(reinitializes,hd,inline,noalias)
      iterator & operator=(iterator rhs) noexcept {
        p = rhs.p;
        i = rhs.i;
        return *this;
      }

      BAD(reinitializes,hd,inline,noalias)
      iterator & operator=(iterator && rhs) noexcept {
        p = std::move(rhs.p);
        i = std::move(rhs.i);
        return *this;
      }

      BAD(hd,inline,pure)
      operator const_iterator() const {
        return { p, i };
      }

      // valid across sources
      BAD(hd,inline,pure)
      friend bool operator ==(iterator lhs, iterator rhs) noexcept {
        assert(lhs.p == rhs.p);
        return lhs.i == rhs.i;
      }

      BAD(hd,inline,pure)
      friend bool operator !=(iterator lhs, iterator rhs) noexcept {
        assert(lhs.p == rhs.p);
        return lhs.i != rhs.i;
      }

      // valid within a single source
      BAD(hd,inline,pure)
      friend bool operator <(iterator lhs, iterator rhs) noexcept {
        assert(lhs.p == rhs.p);
        return lhs.i < rhs.i;
      }

      BAD(hd,inline,pure)
      friend bool operator >(iterator lhs, iterator rhs) noexcept {
        assert(lhs.p == rhs.p);
        return lhs.i > rhs.i;
      }

      BAD(hd,inline,pure)
      friend bool operator <=(iterator lhs, iterator rhs) noexcept {
        assert(lhs.p == rhs.p);
        return lhs.i <= rhs.i;
      }

      BAD(hd,inline,pure)
      friend bool operator >=(iterator lhs, iterator rhs) noexcept {
        assert(lhs.p == rhs.p);
        return lhs.i >= rhs.i;
      }

      BAD(hd,inline,noalias)
      iterator & operator ++() noexcept {
        ++i;
        return *this;
      }

      BAD(hd,inline,noalias)
      iterator operator ++(int) noexcept {
        return { p, i++ };
      }

      BAD(hd,inline,noalias)
      iterator & operator --() noexcept {
        --i;
        return *this;
      }

      BAD(hd,inline,noalias)
      iterator operator --(int) noexcept {
        return { p, i-- };
      }

      BAD(hd,inline,pure)
      friend iterator operator +(iterator lhs, ptrdiff_t rhs) noexcept {
        return { lhs.p, lhs.i + rhs };
      }

      BAD(hd,inline,pure)
      friend iterator operator -(iterator lhs, ptrdiff_t rhs) noexcept {
        return { lhs.p, lhs.i - rhs };
      }

      BAD(hd,inline,pure)
      friend ptrdiff_t operator -(iterator lhs, iterator rhs) noexcept {
        assert(lhs.p == rhs.p);
        return lhs.i - rhs.i;
      }

      BAD(hd,inline,noalias)
      iterator & operator +=(ptrdiff_t rhs) const noexcept {
        i += rhs;
        return *this;
      }

      BAD(hd,inline,noalias)
      iterator & operator -=(ptrdiff_t rhs) const noexcept {
        i -= rhs;
        return *this;
      }

      BAD(hd,inline,pure)
      reference operator *() const noexcept {
        assert(valid());
        return p->at(i);
      }

      BAD(hd,inline,pure)
      pointer operator ->() const noexcept {
        assert(valid());
        return *(p->at(i));
      }

      BAD(hd,inline,pure)
      reference operator[](ptrdiff_t di) const noexcept {
        assert(valid());
        return p->at(i + di);
      }

      BAD(hd,inline,pure)
      bool valid() const noexcept {
        return p != nullptr && 0 <= i && i < d;
      }

      friend iterator operator + (ptrdiff_t lhs, iterator & rhs) {
        return { rhs.p, rhs.i + lhs };
      }
    };

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    BAD(hd,inline,const)
    iterator begin() noexcept {
      return iterator(&at(), 0);
    }

    BAD(hd,inline,const)
    iterator end() noexcept {
      return iterator(&at(), d);
    }

    BAD(hd,inline,const)
    reverse_iterator rbegin() noexcept {
      return reverse_iterator(iterator(&at(), d-1));
    }

    BAD(hd,inline,const)
    reverse_iterator rend() noexcept {
      return reverse_iterator(iterator(&at(), -1));
    }

    BAD(hd,inline,const)
    const_iterator begin() const noexcept {
      return const_iterator(&at(), 0);
    }

    BAD(hd,inline,const)
    const_iterator end() const noexcept {
      return const_iterator(&at(), d);
    }

    BAD(hd,inline,const)
    const_iterator cbegin() const noexcept {
      return const_iterator(&at(), 0);
    }

    BAD(hd,inline,const)
    const_iterator cend() const noexcept {
      return const_iterator(&at(), d);
    }

    BAD(hd,inline,const)
    const_reverse_iterator rbegin() const noexcept {
      return reverse_iterator(const_iterator(&at(), d-1));
    }

    BAD(hd,inline,const)
    const_reverse_iterator rend() const noexcept {
      return reverse_iterator(const_iterator(&at(), -1));
    }

    BAD(hd,inline,const)
    const_reverse_iterator crbegin() const noexcept {
      return reverse_iterator(const_iterator(&at(), d-1));
    }

    BAD(hd,inline,const)
    const_reverse_iterator crend() const noexcept {
      return reverse_iterator(const_iterator(&at(), -1));
    }
  };

  template <class B, size_t d, size_t...ds>
  struct store_rep_expr : store_expr<store_rep_expr<B,d,ds...>,d,ds...> {
    using element = typename B::element;
    using dim = seq<d,ds...>;
    static constexpr size_t arity = 1 + sizeof...(ds);

    B const & base;

    BAD(hd,inline,pure)
    auto operator[](BAD(maybe_unused) size_t i) const noexcept {
      return base;
    }

    template <size_t N>
    BAD(hd,inline,flatten)
    auto pull(size_t i) const noexcept {
      if constexpr(N == 0) {
        return base;
      } else {
        return base.template pull<N-1>(i).template rep<d>();
      }
    }

    // this is lifetimebound, move out of line?
    template <size_t N>
    BAD(hd,inline,flatten,const)
    auto rep() const noexcept -> store_rep_expr<store_rep_expr,N,d,ds...> {
      return { *this };
    }

    BAD(hd)
    friend std::ostream & operator<<(std::ostream & os, store_rep_expr const & rhs) {
      return os << "rep<" << d << ">(" << rhs << ")";
    }
  };

  template <class L, class R, size_t d, size_t... ds>
  struct store_add_expr : store_expr<store_add_expr<L,R,d,ds...>,d,ds...> {
    using dim = seq<d,ds...>;
    using element = decltype(std::declval<typename L::element>() + std::declval<typename R::element>());

    L const & l;
    R const & r;

    BAD(hd)
    store_add_expr(store_expr<L,d,ds...> const & l, store_expr<R,d,ds...> const & r) : l(l()), r(r()) {}

    BAD(hd,inline,pure)
    auto operator[](BAD(maybe_unused) size_t i) const noexcept {
      return l[i] + r[i];
    }

    template <size_t N>
    BAD(hd,inline,flatten)
    auto pull(size_t i) const noexcept {
      return l.template pull<N>(i) + r.template pull<N>(i);
    }

    template <size_t N>
    BAD(hd,inline,flatten,const)
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

  template <size_t d0, class B, size_t d1, size_t... ds>
  BAD(hd,inline)
  auto rep(BAD(lifetimebound) store_expr<B,d1,ds...> const & x) noexcept {
    return x().template rep<d0>();
  }

  template <size_t d0, class B, size_t d1, size_t... ds>
  BAD(hd,inline)
  auto rep(BAD(lifetimebound) store_expr<B,d1,ds...> & x) noexcept {
    return x().template rep<d0>();
  }


  // * pull the nth dimension of a storage expression to the front.

  template <size_t N, class B, size_t d, size_t... ds>
  BAD(hd,inline)
  auto pull(
    BAD(lifetimebound) store_expr<B,d,ds...> const & rhs,
    size_t i
  ) noexcept {
    return rhs().template pull<N>(i);
  }

  // * pull the nth dimension of a store to the front.

  template <size_t N, class B, size_t d, size_t... ds>
  BAD(hd,inline)
  auto pull(
    BAD(lifetimebound) store_expr<B,d,ds...> & rhs,
    size_t i
  ) noexcept {
    return rhs().template pull<N>(i);
  }
}