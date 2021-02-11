#pragma once
#include "namespace.hh"

/// \file
/// \author Edward Kmett
/// \brief store_expr_iterator and const_store_expr_iterator implementations

/// \{

namespace bad::storage {
  /// \ingroup storage_group
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
    BAD(hd,nodiscard,inline,pure) constexpr
    friend bool operator ==(const_store_expr_iterator lhs, const_store_expr_iterator rhs) noexcept {
      assert(lhs.p == rhs.p);
      return lhs.i == rhs.i;
    }

    // NB: store_expr_iterators are only comparable if they come from the same container
    BAD(hd,nodiscard,inline,pure) constexpr
    friend bool operator !=(const_store_expr_iterator lhs, const_store_expr_iterator rhs) noexcept {
      assert(lhs.p == rhs.p);
      return lhs.i != rhs.i;
    }

    BAD(hd,nodiscard,inline,pure) constexpr
    friend bool operator <(const_store_expr_iterator lhs, const_store_expr_iterator rhs) noexcept {
      assert(lhs.p == rhs.p);
      return lhs.i < rhs.i;
    }

    BAD(hd,nodiscard,inline,pure) constexpr
    friend bool operator >(const_store_expr_iterator lhs, const_store_expr_iterator rhs) noexcept {
      assert(lhs.p == rhs.p);
      return lhs.i > rhs.i;
    }

    BAD(hd,nodiscard,inline,pure) constexpr
    friend bool operator <=(const_store_expr_iterator lhs, const_store_expr_iterator rhs) noexcept {
      assert(lhs.p == rhs.p);
      return lhs.i <= rhs.i;
    }

    BAD(hd,nodiscard,inline,pure) constexpr
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

    BAD(hd,nodiscard,inline,pure) constexpr
    friend const_store_expr_iterator operator +(const_store_expr_iterator lhs, ptrdiff_t rhs) noexcept {
      return { lhs.p, lhs.i + rhs };
    }

    BAD(hd,nodiscard,inline,pure) constexpr
    friend const_store_expr_iterator operator +(ptrdiff_t lhs, const_store_expr_iterator rhs) noexcept {
      return { rhs.p, rhs.i + lhs };
    }

    BAD(hd,nodiscard,inline,pure) constexpr
    friend const_store_expr_iterator operator -(const_store_expr_iterator lhs, ptrdiff_t rhs) noexcept {
      return { lhs.p, lhs.i - rhs };
    }

    BAD(hd,nodiscard,inline,pure) constexpr
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

    BAD(hd,nodiscard,inline,pure)
    auto & operator *() const noexcept {
      assert(valid());
      return p->at(i);
    }

    BAD(hd,nodiscard,inline,pure)
    auto * operator ->() const noexcept {
      assert(valid());
      return &(p->at(i));
    }

    BAD(hd,nodiscard,inline,pure)
    auto & operator[](ptrdiff_t di) const noexcept {
      assert(p && 0 <= i + di && i + di < d);
      return p->at(i + di);
    }

    BAD(hd,nodiscard,inline,pure) constexpr
    bool valid() const noexcept {
      return p != nullptr && 0 <= i && i < d;
    }
  };

  /// \ingroup storage_group
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

    BAD(hd,inline,pure) constexpr
    operator const_store_expr_iterator<B,d>() const {
      return { p, i };
    }

    // valid across sources
    BAD(hd,nodiscard,inline,pure) constexpr
    friend bool operator ==(store_expr_iterator lhs, store_expr_iterator rhs) noexcept {
      assert(lhs.p == rhs.p);
      return lhs.i == rhs.i;
    }

    BAD(hd,nodiscard,inline,pure) constexpr
    friend bool operator !=(store_expr_iterator lhs, store_expr_iterator rhs) noexcept {
      assert(lhs.p == rhs.p);
      return lhs.i != rhs.i;
    }

    // valid within a single source
    BAD(hd,nodiscard,inline,pure) constexpr
    friend bool operator <(store_expr_iterator lhs, store_expr_iterator rhs) noexcept {
      assert(lhs.p == rhs.p);
      return lhs.i < rhs.i;
    }

    BAD(hd,nodiscard,inline,pure) constexpr
    friend bool operator >(store_expr_iterator lhs, store_expr_iterator rhs) noexcept {
      assert(lhs.p == rhs.p);
      return lhs.i > rhs.i;
    }

    BAD(hd,nodiscard,inline,pure) constexpr
    friend bool operator <=(store_expr_iterator lhs, store_expr_iterator rhs) noexcept {
      assert(lhs.p == rhs.p);
      return lhs.i <= rhs.i;
    }

    BAD(hd,nodiscard,inline,pure) constexpr
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

    BAD(hd,nodiscard,inline,pure) constexpr
    friend store_expr_iterator operator +(store_expr_iterator lhs, ptrdiff_t rhs) noexcept {
      return { lhs.p, lhs.i + rhs };
    }

    BAD(hd,nodiscard,inline,pure) constexpr
    friend store_expr_iterator operator -(store_expr_iterator lhs, ptrdiff_t rhs) noexcept {
      return { lhs.p, lhs.i - rhs };
    }

    BAD(hd,nodiscard,inline,pure) constexpr
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

    BAD(hd,nodiscard,inline,pure) constexpr
    reference operator *() const noexcept {
      assert(valid());
      return p->at(i);
    }

    BAD(hd,nodiscard,inline,pure) constexpr
    pointer operator ->() const noexcept {
      assert(valid());
      return *(p->at(i));
    }

    BAD(hd,nodiscard,inline,pure) constexpr
    reference operator[](ptrdiff_t di) const noexcept {
      assert(valid());
      return p->at(i + di);
    }

    BAD(hd,nodiscard,inline,pure) constexpr
    bool valid() const noexcept {
      return p != nullptr && 0 <= i && i < d;
    }

    BAD(hd,nodiscard,inline,pure) constexpr
    friend store_expr_iterator operator + (ptrdiff_t lhs, store_expr_iterator rhs) noexcept {
      return { rhs.p, rhs.i + lhs };
    }
  };
}

/// \}
