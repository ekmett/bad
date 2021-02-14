#ifndef BAD_MEMORY_WEAK_INTRUSIVE_PTR_HH
#define BAD_MEMORY_WEAK_INTRUSIVE_PTR_HH

#include "bad/common.hh"
#include "bad/memory/intrusive_ptr.hh"

namespace bad::memory {

  /// \ingroup intrusive_group
  template <class T>
  struct BAD(empty_bases) weak_intrusive_ptr final 
  : totally_ordered<weak_intrusive_ptr<T>>
  , totally_ordered<weak_intrusive_ptr<T>, intrusive_ptr<T>>
  , totally_ordered<weak_intrusive_ptr<T>, T*> {
  private:
    using ptr = decltype(intrusive_ptr(std::declval<T>().make_weak()));
    friend std::hash<weak_intrusive_ptr<T>>;
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
    weak_intrusive_ptr & operator = (T * rhs) noexcept {
      p = rhs ? rhs->make_weak() : nullptr;
      return *this;
    }

    BAD(hd,inline)
    intrusive_ptr<T> reclaim() const noexcept {
      return p ? p->reclaim() : nullptr;
    }

    BAD(hd,reinitializes,inline)
    void reset() noexcept {
      p = nullptr;
    }

    BAD(hd,reinitializes,inline)
    void reset(T * rhs) noexcept {
      p = rhs ? rhs->make_weak() : nullptr;
    }

    BAD(hd,inline) constexpr
    friend bool operator == (
      BAD(noescape) weak_intrusive_ptr const & lhs,
      BAD(noescape) weak_intrusive_ptr const & rhs
    ) noexcept {
      return lhs.p == rhs.p;
    } 

    BAD(hd,inline) constexpr
    friend bool operator == (
      BAD(noescape) weak_intrusive_ptr const & lhs,
      BAD(noescape) T * rhs
    ) noexcept {
      return lhs.p == (rhs ? rhs->make_weak() : nullptr);
    } 

    BAD(hd,inline) constexpr
    friend bool operator == (
      BAD(noescape) weak_intrusive_ptr const & lhs,
      BAD(noescape) intrusive_ptr<T> const & rhs
    ) noexcept {
      return lhs.p == (rhs ? rhs->make_weak() : nullptr);
    } 

    BAD(hd,inline) constexpr
    friend bool operator < (
      BAD(noescape) weak_intrusive_ptr const & lhs,
      BAD(noescape) weak_intrusive_ptr const & rhs
    ) noexcept {
      return lhs.p < rhs.p;
    } 

    BAD(hd,inline) constexpr
    friend bool operator < (
      BAD(noescape) weak_intrusive_ptr const & lhs,
      BAD(noescape) T * rhs
    ) noexcept {
      return lhs.p < (rhs ? rhs->make_weak() : nullptr);
    } 

    BAD(hd,inline) constexpr
    friend bool operator < (
      BAD(noescape) weak_intrusive_ptr const & lhs,
      BAD(noescape) intrusive_ptr<T> const & rhs
    ) noexcept {
      return lhs.p < (rhs ? rhs->make_weak() : nullptr);
    } 

    BAD(hd,inline) constexpr
    friend bool operator > (
      BAD(noescape) weak_intrusive_ptr const & lhs,
      BAD(noescape) weak_intrusive_ptr const & rhs
    ) noexcept {
      return lhs.p > rhs.p;
    } 

    BAD(hd,inline) constexpr
    friend bool operator > (
      BAD(noescape) weak_intrusive_ptr const & lhs,
      BAD(noescape) T * rhs
    ) noexcept {
      return lhs.p > (rhs ? rhs->make_weak() : nullptr);
    } 

    BAD(hd,inline) constexpr
    friend bool operator > (
      BAD(noescape) weak_intrusive_ptr const & lhs,
      BAD(noescape) intrusive_ptr<T> const & rhs
    ) noexcept {
      return lhs.p > (rhs ? rhs->make_weak() : nullptr);
    } 

    BAD(hd,inline)
    void swap(BAD(noescape) weak_intrusive_ptr & rhs) noexcept {
      using std::swap;
      swap(p,rhs.p);
    }
  };

  /// \ingroup intrusive_group
  template <class T>
  BAD(hd,inline)
  void swap(
    BAD(noescape) weak_intrusive_ptr<T> & lhs,
    BAD(noescape) weak_intrusive_ptr<T> & rhs
  ) noexcept {
    lhs.swap(rhs);
  }
}

namespace std {
  /// \ingroup intrusive_group
  template <class T>
  struct BAD(empty_bases) hash<bad::weak_intrusive_ptr<T>> {
    BAD(hd,nodiscard,inline)
    std::size_t operator()(
      BAD(noescape) bad::weak_intrusive_ptr<T> const & p
    ) const noexcept {
      return std::hash<typename bad::weak_intrusive_ptr<T>::ptr>{}(p.p);
    }
  };
}

namespace bad {
  using namespace memory;
}

#endif
