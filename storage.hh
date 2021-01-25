#pragma once
#include <array>
#include "sequences.hh"
#include "attributes.hh"
#include "errors.hh"

/// @file storage.hh
/// @brief tensor storage
/// @author Edward Kmett

#pragma STDC FP_CONTRACT ON

namespace bad {
  namespace storage {
    /// re-exported by \ref bad and \ref bad::storage::api
    namespace common{}
    /// public components
    namespace api { using namespace common; }
    using namespace api;
  }
  using namespace storage::common;
}

/// @defgroup storage storage
/// @brief tensor storage and expressions
/// @{

/// tensor storage
namespace bad::storage {
  using namespace sequences::api;
  using namespace errors::api;

  template <class B, size_t d>
  struct const_store_expr_iterator {
    B const * p;
    ptrdiff_t i;

    using iterator_category = std::random_access_iterator_tag;
    using value_type        = std::add_const<std::remove_reference<decltype(p->at(0))>>;
    using difference_type   = ptrdiff_t;
    using reference_type    = std::add_lvalue_reference<value_type>;
    using pointer_type      = std::add_pointer<value_type>;

    BAD(reinitializes,hd,inline,noalias)
    const_store_expr_iterator & operator =(const_store_expr_iterator rhs) noexcept {
      p = rhs.p;
      i = rhs.i;
      return *this;
    }

    BAD(reinitializes,hd,inline,noalias)
    const_store_expr_iterator & operator =(const_store_expr_iterator && rhs) noexcept {
      p = std::move(rhs.p);
      i = std::move(rhs.i);
      return *this;
    }

    // NB: store_expr_iterators are only comparable if they come from the same container
    BAD(hd,inline,pure)
    friend bool operator ==(const_store_expr_iterator lhs, const_store_expr_iterator rhs) noexcept {
      assert(lhs.p == rhs.p);
      return lhs.i == rhs.i;
    }

    // NB: store_expr_iterators are only comparable if they come from the same container
    BAD(hd,inline,pure)
    friend bool operator !=(const_store_expr_iterator lhs, const_store_expr_iterator rhs) noexcept {
      assert(lhs.p == rhs.p);
      return lhs.i != rhs.i;
    }

    BAD(hd,inline,pure)
    friend bool operator <(const_store_expr_iterator lhs, const_store_expr_iterator rhs) noexcept {
      assert(lhs.p == rhs.p);
      return lhs.i < rhs.i;
    }

    BAD(hd,inline,pure)
    friend bool operator >(const_store_expr_iterator lhs, const_store_expr_iterator rhs) noexcept {
      assert(lhs.p == rhs.p);
      return lhs.i > rhs.i;
    }

    BAD(hd,inline,pure)
    friend bool operator <=(const_store_expr_iterator lhs, const_store_expr_iterator rhs) noexcept {
      assert(lhs.p == rhs.p);
      return lhs.i <= rhs.i;
    }

    BAD(hd,inline,pure)
    friend bool operator >=(const_store_expr_iterator lhs, const_store_expr_iterator rhs) noexcept {
      assert(lhs.p == rhs.p);
      return lhs.i >= rhs.i;
    }

    BAD(hd,inline,noalias)
    const_store_expr_iterator & operator ++() noexcept {
      ++i;
      return *this;
    }

    BAD(hd,inline,noalias)
    const_store_expr_iterator operator ++(int) noexcept {
      return { p, i++ };
    }

    BAD(hd,inline,noalias)
    const_store_expr_iterator & operator --() noexcept {
      --i;
      return *this;
    }

    BAD(hd,inline,noalias)
    const_store_expr_iterator operator --(int) noexcept {
      return { p, i-- };
    }

    BAD(hd,inline,pure)
    friend const_store_expr_iterator operator +(const_store_expr_iterator lhs, ptrdiff_t rhs) noexcept {
      return { lhs.p, lhs.i + rhs };
    }

    friend const_store_expr_iterator operator +(ptrdiff_t lhs, const_store_expr_iterator rhs) noexcept {
      return { rhs.p, rhs.i + lhs };
    }

    BAD(hd,inline,pure)
    friend const_store_expr_iterator operator -(const_store_expr_iterator lhs, ptrdiff_t rhs) noexcept {
      return { lhs.p, lhs.i - rhs };
    }

    BAD(hd,inline,pure)
    friend ptrdiff_t operator -(const_store_expr_iterator lhs, const_store_expr_iterator rhs) noexcept {
      assert(lhs.p == rhs.p);
      return lhs.i - rhs.i;
    }

    BAD(hd,inline,noalias)
    const_store_expr_iterator & operator +=(ptrdiff_t rhs) const noexcept {
      i += rhs;
      return *this;
    }

    BAD(hd,inline,noalias)
    const_store_expr_iterator & operator -=(ptrdiff_t rhs) const noexcept {
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
    auto & operator[](ptrdiff_t di) const noexcept {
      assert(p && 0 <= i + di && i + di < d);
      return p->at(i + di);
    }

    BAD(hd,inline,pure)
    bool valid() const noexcept {
      return p != nullptr && 0 <= i && i < d;
    }
  };

  template <class B, size_t d> 
  struct store_expr_iterator {
    B * p;
    ptrdiff_t i;

    using iterator_category = std::random_access_iterator_tag;
    using value_type        = std::remove_reference<decltype(p->at(0))>;
    using difference_type   = ptrdiff_t;
    using reference         = std::add_lvalue_reference<value_type>;
    using pointer           = std::add_pointer<value_type>;

    BAD(hd,inline,noalias) constexpr
    store_expr_iterator(store_expr_iterator const & rhs) noexcept
    : p(rhs.p), i(rhs.i) {}

    BAD(hd,inline,noalias) constexpr
    store_expr_iterator(store_expr_iterator && rhs) noexcept
    : p(std::move(rhs.p)), i(std::move(rhs.i)) {}

    BAD(reinitializes,hd,inline,noalias)
    store_expr_iterator & operator=(store_expr_iterator rhs) noexcept {
      p = rhs.p;
      i = rhs.i;
      return *this;
    }

    BAD(reinitializes,hd,inline,noalias)
    store_expr_iterator & operator=(store_expr_iterator && rhs) noexcept {
      p = std::move(rhs.p);
      i = std::move(rhs.i);
      return *this;
    }

    BAD(hd,inline,pure)
    operator const_store_expr_iterator<B,d>() const {
      return { p, i };
    }

    // valid across sources
    BAD(hd,inline,pure)
    friend bool operator ==(store_expr_iterator lhs, store_expr_iterator rhs) noexcept {
      assert(lhs.p == rhs.p);
      return lhs.i == rhs.i;
    }

    BAD(hd,inline,pure)
    friend bool operator !=(store_expr_iterator lhs, store_expr_iterator rhs) noexcept {
      assert(lhs.p == rhs.p);
      return lhs.i != rhs.i;
    }

    // valid within a single source
    BAD(hd,inline,pure)
    friend bool operator <(store_expr_iterator lhs, store_expr_iterator rhs) noexcept {
      assert(lhs.p == rhs.p);
      return lhs.i < rhs.i;
    }

    BAD(hd,inline,pure)
    friend bool operator >(store_expr_iterator lhs, store_expr_iterator rhs) noexcept {
      assert(lhs.p == rhs.p);
      return lhs.i > rhs.i;
    }

    BAD(hd,inline,pure)
    friend bool operator <=(store_expr_iterator lhs, store_expr_iterator rhs) noexcept {
      assert(lhs.p == rhs.p);
      return lhs.i <= rhs.i;
    }

    BAD(hd,inline,pure)
    friend bool operator >=(store_expr_iterator lhs, store_expr_iterator rhs) noexcept {
      assert(lhs.p == rhs.p);
      return lhs.i >= rhs.i;
    }

    BAD(hd,inline,noalias)
    store_expr_iterator & operator ++() noexcept {
      ++i;
      return *this;
    }

    BAD(hd,inline,noalias)
    store_expr_iterator operator ++(int) noexcept {
      return { p, i++ };
    }

    BAD(hd,inline,noalias)
    store_expr_iterator & operator --() noexcept {
      --i;
      return *this;
    }

    BAD(hd,inline,noalias)
    store_expr_iterator operator --(int) noexcept {
      return { p, i-- };
    }

    BAD(hd,inline,pure)
    friend store_expr_iterator operator +(store_expr_iterator lhs, ptrdiff_t rhs) noexcept {
      return { lhs.p, lhs.i + rhs };
    }

    BAD(hd,inline,pure)
    friend store_expr_iterator operator -(store_expr_iterator lhs, ptrdiff_t rhs) noexcept {
      return { lhs.p, lhs.i - rhs };
    }

    BAD(hd,inline,pure)
    friend ptrdiff_t operator -(store_expr_iterator lhs, store_expr_iterator rhs) noexcept {
      assert(lhs.p == rhs.p);
      return lhs.i - rhs.i;
    }

    BAD(hd,inline,noalias)
    store_expr_iterator & operator +=(ptrdiff_t rhs) const noexcept {
      i += rhs;
      return *this;
    }

    BAD(hd,inline,noalias)
    store_expr_iterator & operator -=(ptrdiff_t rhs) const noexcept {
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

    BAD(hd,inline,pure)
    friend store_expr_iterator operator + (ptrdiff_t lhs, store_expr_iterator rhs) noexcept {
      return { rhs.p, rhs.i + lhs };
    }
  };
}

namespace bad::storage::api {
  /// variadic expression template
  /// @ingroup storage
  template <class B, size_t d, size_t... ds>
  struct store_expr {
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
    BAD(hd,inline)
    bool operator==(expr<C> & r) const noexcept {
      auto l = at();
      for (size_t i=0;i<d;++i) {
        if (l[i] != r[i]) return false;
      }
      return true;
    }

    template <class C>
    BAD(hd,inline)
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

  /// @ingroup storage
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

  /// @ingroup storage
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

  /// @ingroup storage
  template <class L, class R, size_t d, size_t... ds>
  BAD(hd,inline,const)
  auto operator +(
    BAD(lifetimebound) store_expr<L,d,ds...> const &l,
    BAD(lifetimebound) store_expr<R,d,ds...> const &r
  ) noexcept {
    return store_add_expr<L,R,d,ds...>(l(),r());
  }
}

namespace bad::storage::common {
  /// @ingroup storage
  template <size_t d0, class B, size_t d1, size_t... ds>
  BAD(hd,inline)
  auto rep(BAD(lifetimebound) store_expr<B,d1,ds...> const & x) noexcept {
    return x().template rep<d0>();
  }

  /// @ingroup storage
  template <size_t d0, class B, size_t d1, size_t... ds>
  BAD(hd,inline)
  auto rep(BAD(lifetimebound) store_expr<B,d1,ds...> & x) noexcept {
    return x().template rep<d0>();
  }

  /// pull the `N`th dimension of a storage expression to the front.
  /// @ingroup storage
  template <size_t N, class B, size_t d, size_t... ds>
  BAD(hd,inline)
  auto pull(
    BAD(lifetimebound) store_expr<B,d,ds...> const & rhs,
    size_t i
  ) noexcept {
    return rhs().template pull<N>(i);
  }

  /// pulls the `N`th dimension of a store to the front.
  /// @ingroup storage
  template <size_t N, class B, size_t d, size_t... ds>
  BAD(hd,inline)
  auto pull(
    BAD(lifetimebound) store_expr<B,d,ds...> & rhs,
    size_t i
  ) noexcept {
    return rhs().template pull<N>(i);
  }
}

namespace bad::storage::common {
  /// @private
  template <class T, class Dim, class Stride = row_major<Dim>>
  struct store {
    // offer slightly more helpful diagnostics first
    static_assert(std::is_same_v<seq_element_type<Dim>,size_t>, "expected dim to have type seq<...>");
    static_assert(std::is_same_v<seq_element_type<Stride>,ptrdiff_t>, "expected stride to have type sseq<...>");
    static_assert(seq_length<Dim> == seq_length<Stride>, "dim and stride have mismatched lengths");
    static_assert(no<T>, "only partial specializations are valid");
  };

  /// @ingroup storage
  template <class T>
  using scalar = store<T,seq<>,sseq<>>;

  /// scalars
  /// @ingroup storage
  template <class T>
  struct store<T, seq<>, sseq<>> {
    using element = T;
    using dim = seq<>;
    using stride = sseq<>;
    static constexpr size_t arity = 0;

    BAD(hd,inline)
    store() : value() {}

    BAD(hd,inline)
    store(T value) : value(value) {}

    BAD(hd,inline)
    store(const store & rhs) : value(rhs.value) {};

    BAD(hd,inline)
    store(store && rhs) : value(std::move(rhs.value)) {}

    BAD(hd,inline)
    store & operator =(const store & rhs) {
      value = rhs.value;
      return *this;
    }

    BAD(hd,inline)
    store & operator =(store && rhs) {
      value = std::move(rhs);
      return *this;
    }

    T value;

    BAD(hd,inline) // const?
    T & at() noexcept { return value; }

    BAD(hd,inline,pure) // const?
    const T & at() const noexcept { return value; }

    template <typename arg, typename... args>
    BAD(hd,inline)
    const T & at(arg i, args... is) const noexcept { return value(i,is...); }

    template <typename arg, typename... args>
    BAD(hd,inline)
    auto at(arg i, args... is) noexcept { return value(i,is...); }


    BAD(hd,inline,pure) // const?
    T & operator()() noexcept { return value; }

    BAD(hd,inline,pure) // const?
    const T & operator()() const noexcept { return value; }

    template <typename arg, typename... args>
    BAD(hd,inline)
    T & operator()(arg i, args... is) noexcept { return value(i,is...); }

    template <typename arg, typename... args>
    BAD(hd,inline)
    const T & operator()(arg i, args... is) const noexcept { return value(i,is...); }

    BAD(hd,inline,pure) // const?
    operator T & () noexcept { return value; }

    BAD(hd,inline,pure) // const?
    operator T const & () const noexcept { return value; }
  };

  /// @ingroup storage
  template <typename T>
  BAD(hd,inline,flatten)
  void swap(
    BAD(noescape) scalar<T> & l,
    BAD(noescape) scalar<T> & r
  ) noexcept {
    using std::swap;
    swap(l.value,r.value);
  }
}

namespace bad::storage {
  template <size_t d, size_t s, class T, class plane>
  struct const_store_iterator {
    using value_type = plane const;
    using difference_type = ptrdiff_t;
    using pointer = value_type *;
    using reference = value_type &;
    using iterator_category = std::random_access_iterator_tag;

    BAD(hd,inline,noalias)
    constexpr explicit const_store_iterator(T const * p = nullptr, ptrdiff_t i = 0) noexcept
    : p(p), i(i) {}

    BAD(hd,inline,noalias)
    constexpr const_store_iterator(const const_store_iterator & rhs) noexcept
    : p(rhs.p), i(rhs.i) {}

    BAD(hd,inline,noalias)
    constexpr const_store_iterator(const_store_iterator && rhs) noexcept
    : p(std::move(rhs.p)), i(std::move(rhs.i)) {}

    BAD(reinitializes,hd,inline,noalias)
    const_store_iterator & operator =(const_store_iterator rhs) noexcept {
      p = rhs.p;
      i = rhs.i;
      return *this;
    }

    BAD(reinitializes,hd,inline,noalias)
    const_store_iterator & operator =(const_store_iterator && rhs) noexcept {
      p = std::move(rhs.p);
      i = std::move(rhs.i);
      return *this;
    }

    // default constructible, moveable
    T const * p;
    ptrdiff_t i;

    // NB: store_iterators are only comparable if they come from the same container
    BAD(hd,inline,pure)
    friend bool operator ==(const_store_iterator lhs, const_store_iterator rhs) noexcept {
      return lhs.i == rhs.i;
    }

    // NB: store_iterators are only comparable if they come from the same container
    BAD(hd,inline,pure)
    friend bool operator !=(const_store_iterator lhs, const_store_iterator rhs) noexcept {
      return lhs.i != rhs.i;
    }

    BAD(hd,inline,pure)
    friend bool operator <(const_store_iterator lhs, const_store_iterator rhs) noexcept {
      return lhs.i < rhs.i;
    }

    BAD(hd,inline,pure)
    friend bool operator >(const_store_iterator lhs, const_store_iterator rhs) noexcept {
      return lhs.i > rhs.i;
    }

    BAD(hd,inline,pure)
    friend bool operator <=(const_store_iterator lhs, const_store_iterator rhs) noexcept {
      return lhs.i <= rhs.i;
    }

    BAD(hd,inline,pure)
    friend bool operator >=(const_store_iterator lhs, const_store_iterator rhs) noexcept {
      return lhs.i >= rhs.i;
    }

    BAD(hd,inline,noalias)
    const_store_iterator & operator ++() noexcept {
      ++i;
      return *this;
    }

    BAD(hd,inline,noalias)
    const_store_iterator operator ++(int) noexcept {
      return { p, i++ };
    }

    BAD(hd,inline,noalias)
    const_store_iterator & operator --() noexcept {
      --i;
      return *this;
    }

    BAD(hd,inline,noalias)
    const_store_iterator operator --(int) noexcept {
      return { p, i-- };
    }

    BAD(hd,inline,pure)
    friend const_store_iterator operator +(const_store_iterator lhs, ptrdiff_t rhs) noexcept {
      return { lhs.p, lhs.i + rhs };
    }

    friend const_store_iterator operator +(ptrdiff_t lhs, const_store_iterator rhs) noexcept {
      return { rhs.p, rhs.i + lhs };
    }

    BAD(hd,inline,pure)
    friend const_store_iterator operator -(const_store_iterator lhs, ptrdiff_t rhs) noexcept {
      return { lhs.p, lhs.i - rhs };
    }

    BAD(hd,inline,pure)
    friend ptrdiff_t operator -(const_store_iterator lhs, const_store_iterator rhs) noexcept {
      assert(lhs.p == rhs.p);
      return lhs.i - rhs.i;
    }

    BAD(hd,inline,noalias)
    const_store_iterator & operator +=(ptrdiff_t rhs) const noexcept {
      i += rhs;
      return *this;
    }

    BAD(hd,inline,noalias)
    const_store_iterator & operator -=(ptrdiff_t rhs) const noexcept {
      i -= rhs;
      return *this;
    }

    BAD(hd,inline,pure)
    reference operator *() const noexcept {
      return *reinterpret_cast<pointer>(p + i*s);
    }

    BAD(hd,inline,pure)
    pointer operator ->() const noexcept {
      return reinterpret_cast<pointer>(p + i*s);
    }

    BAD(hd,inline,pure)
    reference operator[](ptrdiff_t di) const noexcept {
      return *reinterpret_cast<pointer>(p + (i+di)*s);
    }

    BAD(hd,inline,pure)
    bool valid() const noexcept {
      return p != nullptr && 0 <= i && i < d;
    }
  };

  template <size_t d, size_t s, class T, class plane>
  struct store_iterator {
    using value_type = plane;
    using difference_type = ptrdiff_t;
    using pointer = value_type *;
    using reference = value_type &;
    using iterator_category = std::random_access_iterator_tag;

    BAD(hd,inline,noalias) constexpr
    explicit store_iterator(
      T* p = nullptr,
      ptrdiff_t i = 0
    ) noexcept
    : p(p), i(i) {}

    BAD(hd,inline,noalias) constexpr
    store_iterator(store_iterator const & rhs) noexcept
    : p(rhs.p), i(rhs.i) {}

    BAD(hd,inline,noalias) constexpr
    store_iterator(store_iterator && rhs) noexcept
    : p(std::move(rhs.p)), i(std::move(rhs.i)) {}

    BAD(reinitializes,hd,inline,noalias)
    store_iterator & operator =(store_iterator rhs) noexcept {
      p = rhs.p;
      i = rhs.i;
      return *this;
    }

    BAD(reinitializes,hd,inline,noalias)
    store_iterator & operator =(store_iterator && rhs) noexcept {
      p = std::move(rhs.p);
      i = std::move(rhs.i);
      return *this;
    }

    T * p;
    ptrdiff_t i;

    BAD(hd,inline,pure)
    operator const_store_iterator<d,s,T,plane>() const {
      return const_store_iterator(p, i);
    }

    // valid across sources
    BAD(hd,inline,pure)
    friend bool operator ==(store_iterator lhs, store_iterator rhs) noexcept {
      return lhs.i == rhs.i;
    }

    BAD(hd,inline,pure)
    friend bool operator !=(store_iterator lhs, store_iterator rhs) noexcept {
      return lhs.i != rhs.i;
    }

    // valid within a single source
    BAD(hd,inline,pure)
    friend bool operator <(store_iterator lhs, store_iterator rhs) noexcept {
      return lhs.i < rhs.i;
    }

    BAD(hd,inline,pure)
    friend bool operator >(store_iterator lhs, store_iterator rhs) noexcept {
      return lhs.i > rhs.i;
    }

    BAD(hd,inline,pure)
    friend bool operator <=(store_iterator lhs, store_iterator rhs) noexcept {
      return lhs.i <= rhs.i;
    }

    BAD(hd,inline,pure)
    friend bool operator >=(store_iterator lhs, store_iterator rhs) noexcept {
      return lhs.i >= rhs.i;
    }

    BAD(hd,inline,noalias)
    store_iterator & operator ++() noexcept {
      ++i;
      return *this;
    }

    BAD(hd,inline,noalias)
    store_iterator operator ++(int) noexcept {
      return store_iterator(p, i++);
    }

    BAD(hd,inline,noalias)
    store_iterator & operator --() noexcept {
      --i;
      return *this;
    }

    BAD(hd,inline,noalias)
    store_iterator operator --(int) noexcept {
      return store_iterator(p, i--);
    }

    BAD(hd,inline,pure)
    friend store_iterator operator +(store_iterator lhs, ptrdiff_t rhs) noexcept {
      return store_iterator(lhs.p, lhs.i + rhs);
    }

    BAD(hd,inline,pure)
    friend store_iterator operator -(store_iterator lhs, ptrdiff_t rhs) noexcept {
      return store_iterator(lhs.p, lhs.i - rhs);
    }

    BAD(hd,inline,pure)
    friend ptrdiff_t operator -(store_iterator lhs, store_iterator rhs) noexcept {
      assert(lhs.p == rhs.p);
      return lhs.i - rhs.i;
    }

    BAD(hd,inline,noalias)
    store_iterator & operator +=(ptrdiff_t rhs) const noexcept {
      i += rhs;
      return *this;
    }

    BAD(hd,inline,noalias)
    store_iterator & operator -=(ptrdiff_t rhs) const noexcept {
      i -= rhs;
      return *this;
    }

    BAD(hd,inline,pure)
    reference operator *() const noexcept {
      return *reinterpret_cast<pointer>(p + i*s);
    }

    BAD(hd,inline,pure)
    pointer operator ->() const noexcept {
      return reinterpret_cast<pointer>(p + i*s);
    }

    BAD(hd,inline,pure)
    reference operator[](ptrdiff_t di) const noexcept {
      return *reinterpret_cast<pointer>(p + (i+di)*s);
    }

    BAD(hd,inline,pure)
    bool valid() const noexcept {
      return 0 <= i && i < d;
    }

    friend store_iterator operator + (ptrdiff_t lhs, store_iterator rhs) {
      return store_iterator(rhs.p, rhs.i + lhs);
    }
  };
}

namespace bad::storage::common {
  /// tensors
  /// @ingroup storage
  template <class T, size_t d, size_t... ds, ptrdiff_t s, ptrdiff_t... ss>
  struct store<T, seq<d,ds...>, sseq<s,ss...>>
  : store_expr<store<T, seq<d,ds...>, sseq<s,ss...>>,d,ds...> {

    using element = T;
    using dim = seq<d,ds...>;
    using stride = sseq<s,ss...>;
    using plane = store<T,seq<ds...>,sseq<ss...>>;
    using iterator = store_iterator<d,s,T,plane>;
    using const_iterator = store_iterator<d,s,T,plane>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    static_assert(sizeof...(ss) == sizeof...(ds),"dim and stride do not have the same number of dimension");

    static constexpr size_t arity = 1 + sizeof...(ds);

    template <size_t i> static constexpr size_t nth_dim = nth<i,d,ds...>;
    template <size_t i> static constexpr ptrdiff_t nth_stride = nth<i,s,ss...>;
    template <size_t i> static constexpr ptrdiff_t nth_extremum = nth_stride<i>*(nth_dim<i>-1);

    template <class B>
    using expr = store_expr<B,d,ds...>;

    using super = expr<store>;

    /// @private
    template <class> struct calc_t;

    /// @private
    template <size_t... is>
    struct calc_t<seq<is...>> {
      static constexpr ptrdiff_t max = (0 + ... + std::max<ptrdiff_t>(0,nth_extremum<is>));
      static constexpr ptrdiff_t min = (0 + ... + std::min<ptrdiff_t>(0,nth_extremum<is>));
    };

    /// @private
    using calc = calc_t<make_seq<arity>>;
    // offset in delta to apply when looking up the nth plane
    // keep in mind planes can have higher strides than we do here!
    static constexpr size_t delta = std::max<ptrdiff_t>(0,s*(1-d));
    //static constexpr size_t offset = - calc::min;
  public:
    static constexpr size_t size = calc::max - calc::min + 1;

    T data[size]; ///< The ONLY data member allowed in this class

    BAD(hd,inline)
    constexpr store() noexcept
    : data() {}

    BAD(hd,inline)
    constexpr store(
      T value
    ) noexcept
    : data() {
      std::fill(begin(),end(),value);
    }

    BAD(hd,inline)
    constexpr store(
      std::initializer_list<T> list
    ) noexcept
    : data() {
      assert(list.size() <= d);
      std::copy(list.begin(),list.end(),begin());
    }

    template <class B>
    BAD(hd,inline)
    constexpr store(expr<B> const & rhs) noexcept {
      for (size_t i=0;i<d;++i)
        at(i) = rhs[i];
    }

    template <class A, class B, class... Cs>
    BAD(hd,inline)
    constexpr store(A a,B b,Cs...cs) noexcept : data() {
      static_assert(sizeof...(cs) + 2 <= d);
      auto i = begin();
      *i++ = a;
      *i++ = b;
      ((*i++ = cs),...,void());
    }

    // this should lifetimebound
    BAD(hd,inline,const)
    plane & operator[](size_t i) noexcept {
      return reinterpret_cast<plane &>(data[delta + i*s]);
    }

    BAD(hd,inline,const)
    plane const & operator[](size_t i) const noexcept {
      return reinterpret_cast<plane const &>(data[delta + i*s]);
    }

    BAD(hd,inline,const)
    store & at() noexcept {
      return *this;
    }

    BAD(hd,inline,const)
    store const & at() const noexcept {
      return *this;
    }

    template <class... ts>
    BAD(hd,inline,flatten) // this lifetimebound
    auto & at(size_t i) noexcept {
      return at()[i];
    }

    template <class... ts>
    BAD(hd,inline,flatten) // this lifetimebound
    auto & at(size_t i) const noexcept {
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
    store & operator()() noexcept {
      return at();
    }

    BAD(hd,inline,const) // this lifetimebound
    store const & operator()() const noexcept {
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

    // this should match the behavior of the default = operator, which is to _0_ extend the initializer list
    // this happens because data is initialized in the initializer_list constructor
    // if the initializer_list too long, complain at runtime. otherwise we have a choice of semantics to 0 extend like arrays or
    BAD(hd,inline)
    store & operator = (
      std::initializer_list<T> list
    ) noexcept {
      assert(list.size() <= d);
      std::copy(list.begin(),list.end(),begin());
      for (size_t i = list.size();i<d; ++ i) {
        at(i) = 0;
      }
      return *this;
    }

    template <class B>
    BAD(reinitializes,hd,inline,flatten)
    store & operator = (expr<B> const & rhs) noexcept {
      for (size_t i=0;i<d;++i)
        at(i) = rhs[i];
      return *this;
    }

    BAD(hd,inline)
    store & operator += (
      std::initializer_list<T> list
    ) noexcept {
      assert(list.size() <= d);
      for (auto i=list.begin(),j=begin();i!=list.end();++i)
        *j += *i;
      return *this;
    }

    template <class B>
    BAD(hd,inline,flatten)
    store & operator += (expr<B> const & rhs) noexcept {
      for (size_t i=0;i<d;++i)
        at(i) += rhs[i];
      return *this;
    }

    BAD(hd,inline)
    store & operator -= (
      std::initializer_list<T> list
    ) noexcept {
      assert(list.size() <= d);
      for (auto i = list.begin(), j = begin();i != list.end();++i)
        *j -= *i;
      return *this;
    }

    template <class B>
    BAD(hd,inline,flatten)
    store & operator -= (expr<B> const & rhs) noexcept {
      for (size_t i=0;i<d;++i)
        at(i) -= rhs[i];
      return *this;
    }

    template <class B>
    BAD(hd,inline,flatten)
    store & operator *= (expr<B> const & rhs) noexcept {
      for (size_t i=0;i<d;++i)
        at(i) *= rhs[i];
      return *this;
    }

    template <size_t N>
    using store_pull = store<T, seq_pull<N,dim>, seq_pull<N,stride>>;

    template <size_t N>
    BAD(hd,inline,const)
    store_pull<N> & pull() noexcept {
      return reinterpret_cast<store_pull<N>&>(*this);
    };

    template <size_t N>
    BAD(hd,inline,const)
    const store_pull<N> & pull() const noexcept {
      return reinterpret_cast<store_pull<N> const &>(*this);
    };

    template <size_t N>
    BAD(hd,inline,flatten)
    typename store_pull<N>::plane & pull(size_t i) noexcept {
      return pull<N>()[i];
    };

    template <size_t N>
    BAD(hd,inline,flatten)
    typename store_pull<N>::plane const & pull(size_t i) const noexcept {
      return pull<N>()[i];
    }

    template <size_t N>
    BAD(hd,inline,const)
    auto & rep() const noexcept {
      using rep_t = store<T,seq_cons<N,dim>,seq_cons<ptrdiff_t(0),stride>>;
      return reinterpret_cast<rep_t const &>(*this);
    }

    template <size_t N>
    BAD(hd,inline,const)
    auto & rep() noexcept {
      using rep_t = store<T,seq_cons<N,dim>,seq_cons<ptrdiff_t(0),stride>>;
      return reinterpret_cast<rep_t &>(*this);
    }

    BAD(hd,inline,const)
    iterator begin() noexcept {
      return iterator(data + delta, 0);
    }

    BAD(hd,inline,const)
    iterator end() noexcept {
      return iterator(data + delta, d);
    }

    BAD(hd,inline,const)
    reverse_iterator rbegin() noexcept {
      return reverse_iterator(iterator(data + delta, d-1));
    }

    BAD(hd,inline,const)
    reverse_iterator rend() noexcept {
      return reverse_iterator(iterator(data + delta, -1));
    }

    BAD(hd,inline,const)
    const_iterator begin() const noexcept {
      return const_iterator(data + delta, 0);
    }

    BAD(hd,inline,const)
    const_iterator end() const noexcept {
      return const_iterator(data + delta, d);
    }

    BAD(hd,inline,const)
    const_iterator cbegin() const noexcept {
      return const_iterator(data + delta, 0);
    }

    BAD(hd,inline,const)
    const_iterator cend() const noexcept {
      return const_iterator(data + delta, d);
    }

    BAD(hd,inline,const)
    const_reverse_iterator rbegin() const noexcept {
      return reverse_iterator(const_iterator(data + delta, d-1));
    }

    BAD(hd,inline,const)
    const_reverse_iterator rend() const noexcept {
      return reverse_iterator(const_iterator(data + delta, -1));
    }

    BAD(hd,inline,const)
    const_reverse_iterator crbegin() const noexcept {
      return reverse_iterator(const_iterator(data + delta, d-1));
    }

    BAD(hd,inline,const)
    const_reverse_iterator crend() const noexcept {
      return reverse_iterator(const_iterator(data + delta, -1));
    }

    BAD(hd)
    friend std::ostream & operator<<(std::ostream &os, store const & rhs) {
      os << "{";
      for (size_t i=0;i<d;++i) {
        if (i) os << ",";
        os << rhs[i];
      }
      return os << "}";
    }
  }; // type

  /// scalar initialization
  /// @ingroup storage
  template <class T>
  store(const T &) -> store<T,seq<>,sseq<>>;

  /// copy stride if copying from another store
  /// @ingroup storage
  template <class T, class dim, class stride>
  store(const store<T,dim,stride> &) -> store<T,dim,stride>;

  /// when fed one store_expression copy its dimensions, and pick a row_major order
  /// @ingroup storage
  template <class B, size_t d, size_t... ds>
  store(const store_expr<B,d,ds...> &) -> store<typename B::element,seq<d,ds...>>; // implicitly row_major

  /// when constructed from multiple storage expressions, use the length of the argument list to build the outermost dimension.
  /// @ingroup storage
  template <class B, size_t d, size_t...ds, class... U>
  store(const store_expr<B,d,ds...> &, U...) -> store<typename B::element,seq<1+sizeof...(U),d,ds...>>;

  /// vector construction
  /// @ingroup storage
  template <class T, class... U>
  store(T, U...) -> store<T,seq<1+sizeof...(U)>>;

  /// synonym, even though \ref store has better inference
  /// @ingroup storage
  template <class T, size_t N>
  using vec = store<T,seq<N>>;

  // for a more general version of the multiple storage expression rule, we'd need to be able to know dimensions for arbitrary types.
  // if we had something like:
  // dim<float> = seq<>, dim<store<T,Stride,Dim>> = Stride
  // dim<store_expr<B,d,ds...>> = seq<d,ds...>
  // base_type<B> to get at B::element as well
  // we could make this more robust
  // take shape and type from first arg, 
 
  /// @ingroup storage
  template <typename T, typename stride1, typename stride2, size_t d, size_t... ds>
  BAD(hd,inline,flatten)
  void swap(
    BAD(noescape) store<T,seq<d,ds...>,stride1> & l,
    BAD(noescape) store<T,seq<d,ds...>,stride2> & r
  ) noexcept {
    using std::swap;
    for (size_t i=0;i<d;++i)
      swap(l[i],r[i]);
  }
} // namespace bad::storage::common

namespace bad::storage::api {
  /// replicate base data types
  /// @ingroup storage
  template <size_t d, class T>
  BAD(hd,inline)
  auto rep(T t) noexcept -> store<T,seq<d>,sseq<0>> {
    return t;
  }

  /// used to show the values in an expr or fixed array
  /// @ingroup storage
  template <size_t d, class T>
  struct show_values {
    T const & data;

    BAD(hd)
    show_values(BAD(lifetimebound) const T & data) noexcept
    : data(data) {};

    BAD(hd)
    show_values(BAD(lifetimebound) const T (&data)[d]) noexcept
    : data(data) {}
  };

  /// @ingroup storage
  template <size_t d, class T>
  BAD(hd)
  std::ostream & operator << (std::ostream &os, const show_values<d,T> & rhs) {
    os << "{";
    for (size_t i=0;i<d;++i) {
      if (i) os << ",";
      os << rhs.data[i];
    }
    return os << "}";
  }

  /// infer \ref show_values size from store_expr dimension
  /// @ingroup storage
  template <size_t d, size_t... ds, class B>
  show_values(store_expr<B,d,ds...> const & data)
    -> show_values<d, store_expr<B,d,ds...>>;

  /// infer \ref show_values size from array size
  /// @ingroup storage
  template <size_t d, class T>
  show_values(T(&)[d])
    -> show_values<d,T*>;
}

/// @}
