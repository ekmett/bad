#ifndef BAD_MEMORY_WEAK_INTRUSIVE_PTR_HH
#define BAD_MEMORY_WEAK_INTRUSIVE_PTR_HH

#include "bad/common.hh"
#include "bad/memory/intrusive_ptr.hh"

namespace bad::memory {

  template <class T>
  struct weak_intrusive_ptr final {
  private:
    using ptr = decltype(intrusive_ptr(std::declval<T>().make_weak()));
    ptr p;

  public:
    BAD(hd,inline) constexpr
    weak_intrusive_ptr() noexcept : p() {}

    BAD(hd,inline)
    weak_intrusive_ptr(weak_intrusive_ptr && rhs) noexcept : p(std::move(rhs.p)) {}

    BAD(hd,inline) constexpr
    weak_intrusive_ptr(weak_intrusive_ptr const & rhs) noexcept : p(rhs.p) {}

    BAD(hd,inline)
    weak_intrusive_ptr(T * v) noexcept : p(v ? v->make_weak() : nullptr) {}

    BAD(hd,inline)
    weak_intrusive_ptr & operator = (weak_intrusive_ptr && rhs) noexcept {
      using std::swap;
      swap(ptr(rhs.p),p);
      return *this;
    }

    BAD(hd,inline)
    weak_intrusive_ptr & operator = (weak_intrusive_ptr const & rhs) noexcept {
      using std::swap;
      swap(ptr(rhs.p),p);
      return *this;
    }

    BAD(hd,inline)
    intrusive_ptr<T> reclaim() const noexcept {
      return p ? p->reclaim() : nullptr;
    }

    BAD(hd,inline) constexpr
    friend bool operator == (
      BAD(noescape) weak_intrusive_ptr const & lhs,
      BAD(noescape) weak_intrusive_ptr const & rhs
    ) noexcept {
      return lhs.p == rhs.p;
    } 

    BAD(hd,inline) constexpr
    friend bool operator != (
      BAD(noescape) weak_intrusive_ptr const & lhs,
      BAD(noescape) weak_intrusive_ptr const & rhs
    ) noexcept {
      return lhs.p != rhs.p;
    } 

    BAD(hd,inline) constexpr
    friend bool operator == (
      BAD(noescape) weak_intrusive_ptr const & lhs,
      BAD(noescape) T * rhs
    ) noexcept {
      return lhs.p == (rhs ? rhs->make_weak() : nullptr);
    } 

    BAD(hd,inline) constexpr
    friend bool operator != (
      BAD(noescape) weak_intrusive_ptr const & lhs,
      BAD(noescape) T * rhs
    ) noexcept {
      return lhs.p != (rhs ? rhs->make_weak() : nullptr);
    } 

    BAD(hd,inline) constexpr
    friend bool operator == (
      BAD(noescape) T * lhs,
      BAD(noescape) weak_intrusive_ptr const & rhs
    ) noexcept {
      return (lhs ? lhs->make_weak() : nullptr) == rhs;
    } 

    BAD(hd,inline) constexpr
    friend bool operator != (
      BAD(noescape) T * lhs,
      BAD(noescape) weak_intrusive_ptr const & rhs
    ) noexcept {
      return (lhs ? lhs->make_weak() : nullptr) == rhs;
    } 
  };
}

namespace bad {
  using namespace memory;
}

#endif
