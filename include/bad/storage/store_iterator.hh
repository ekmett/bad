#ifndef BAD_STORAGE_STORE_ITERATOR_HH
#define BAD_STORAGE_STORE_ITERATOR_HH

#include "bad/storage/namespace.hh"

/// \file
/// \author Edward Kmett
/// \brief const_store_expr_iterator and store_expr_iterator implementations

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
    constexpr const_store_iterator(const_store_iterator const & rhs) noexcept
    : p(rhs.p), i(rhs.i) {}

    BAD(hd,inline,noalias)
    constexpr const_store_iterator(const_store_iterator && rhs) noexcept
    : p(std::move(rhs.p)), i(std::move(rhs.i)) {}

    BAD(reinitializes,hd,inline,noalias)
    const_store_iterator & operator =(
      const_store_iterator rhs
    ) noexcept {
      p = rhs.p;
      i = rhs.i;
      return *this;
    }

    BAD(reinitializes,hd,inline,noalias)
    const_store_iterator & operator =(
      const_store_iterator && rhs
    ) noexcept {
      p = std::move(rhs.p);
      i = std::move(rhs.i);
      return *this;
    }

    // default constructible, moveable
    T const * p;
    ptrdiff_t i;

    // NB: store_iterators are only comparable if they come from the same container
    BAD(hd,nodiscard,inline,pure)
    friend bool operator ==(
      const_store_iterator lhs,
      const_store_iterator rhs
    ) noexcept {
      return lhs.i == rhs.i;
    }

    // NB: store_iterators are only comparable if they come from the same container
    BAD(hd,nodiscard,inline,pure)
    friend bool operator !=(
      const_store_iterator lhs,
      const_store_iterator rhs
    ) noexcept {
      return lhs.i != rhs.i;
    }

    BAD(hd,nodiscard,inline,pure)
    friend bool operator <(
      const_store_iterator lhs,
      const_store_iterator rhs
    ) noexcept {
      return lhs.i < rhs.i;
    }

    BAD(hd,nodiscard,inline,pure)
    friend bool operator >(
      const_store_iterator lhs,
      const_store_iterator rhs
    ) noexcept {
      return lhs.i > rhs.i;
    }

    BAD(hd,nodiscard,inline,pure)
    friend bool operator <=(
      const_store_iterator lhs,
      const_store_iterator rhs
    ) noexcept {
      return lhs.i <= rhs.i;
    }

    BAD(hd,nodiscard,inline,pure)
    friend bool operator >=(
      const_store_iterator lhs,
      const_store_iterator rhs
    ) noexcept {
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

    BAD(hd,nodiscard,inline,pure)
    friend const_store_iterator operator +(
      const_store_iterator lhs,
      ptrdiff_t rhs
    ) noexcept {
      return { lhs.p, lhs.i + rhs };
    }

    BAD(hd,nodiscard,inline,pure)
    friend const_store_iterator operator +(
      ptrdiff_t lhs,
      const_store_iterator rhs
    ) noexcept {
      return { rhs.p, rhs.i + lhs };
    }

    BAD(hd,nodiscard,inline,pure)
    friend const_store_iterator operator -(
      const_store_iterator lhs,
      ptrdiff_t rhs
    ) noexcept {
      return { lhs.p, lhs.i - rhs };
    }

    BAD(hd,nodiscard,inline,pure)
    friend ptrdiff_t operator -(
      const_store_iterator lhs,
      const_store_iterator rhs
    ) noexcept {
      assert(lhs.p == rhs.p);
      return lhs.i - rhs.i;
    }

    BAD(hd,inline,noalias)
    const_store_iterator & operator +=(
      ptrdiff_t rhs
    ) const noexcept {
      i += rhs;
      return *this;
    }

    BAD(hd,inline,noalias)
    const_store_iterator & operator -=(
      ptrdiff_t rhs
    ) const noexcept {
      i -= rhs;
      return *this;
    }

    BAD(hd,nodiscard,inline,pure)
    reference operator *() const noexcept {
      return *reinterpret_cast<pointer>(p + i*s);
    }

    BAD(hd,nodiscard,inline,pure)
    pointer operator ->() const noexcept {
      return reinterpret_cast<pointer>(p + i*s);
    }

    BAD(hd,nodiscard,inline,pure)
    reference operator[](
      ptrdiff_t di
    ) const noexcept {
      return *reinterpret_cast<pointer>(p + (i+di)*s);
    }

    BAD(hd,nodiscard,inline,pure)
    bool valid() const noexcept {
      return p != nullptr && 0 <= i && i < d;
    }
  };

  /// \ingroup storage_group
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

    BAD(hd,nodiscard,inline,pure)
    operator const_store_iterator<d,s,T,plane>() const {
      return const_store_iterator(p, i);
    }

    // valid across sources
    BAD(hd,nodiscard,inline,pure)
    friend bool operator ==(store_iterator lhs, store_iterator rhs) noexcept {
      return lhs.i == rhs.i;
    }

    BAD(hd,nodiscard,inline,pure)
    friend bool operator !=(store_iterator lhs, store_iterator rhs) noexcept {
      return lhs.i != rhs.i;
    }

    // valid within a single source
    BAD(hd,nodiscard,inline,pure)
    friend bool operator <(store_iterator lhs, store_iterator rhs) noexcept {
      return lhs.i < rhs.i;
    }

    BAD(hd,nodiscard,inline,pure)
    friend bool operator >(store_iterator lhs, store_iterator rhs) noexcept {
      return lhs.i > rhs.i;
    }

    BAD(hd,nodiscard,inline,pure)
    friend bool operator <=(store_iterator lhs, store_iterator rhs) noexcept {
      return lhs.i <= rhs.i;
    }

    BAD(hd,nodiscard,inline,pure)
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

    BAD(hd,nodiscard,inline,pure)
    friend store_iterator operator +(store_iterator lhs, ptrdiff_t rhs) noexcept {
      return store_iterator(lhs.p, lhs.i + rhs);
    }

    BAD(hd,nodiscard,inline,pure)
    friend store_iterator operator -(store_iterator lhs, ptrdiff_t rhs) noexcept {
      return store_iterator(lhs.p, lhs.i - rhs);
    }

    BAD(hd,nodiscard,inline,pure)
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

    BAD(hd,nodiscard,inline,pure)
    reference operator *() const noexcept {
      return *reinterpret_cast<pointer>(p + i*s);
    }

    BAD(hd,nodiscard,inline,pure)
    pointer operator ->() const noexcept {
      return reinterpret_cast<pointer>(p + i*s);
    }

    BAD(hd,nodiscard,inline,pure)
    reference operator[](ptrdiff_t di) const noexcept {
      return *reinterpret_cast<pointer>(p + (i+di)*s);
    }

    BAD(hd,nodiscard,inline,pure)
    bool valid() const noexcept {
      return 0 <= i && i < d;
    }

    BAD(hd,nodiscard,inline,pure)
    friend store_iterator operator + (ptrdiff_t lhs, store_iterator rhs) {
      return store_iterator(rhs.p, rhs.i + lhs);
    }
  };
}

#endif
