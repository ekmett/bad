#ifndef BAD_MEMORY_INTRUSIVE_PTR_HH
#define BAD_MEMORY_INTRUSIVE_PTR_HH

#include <iosfwd>

#include "bad/attributes.hh"

namespace bad::memory {

  // reference counted intrusive pointers
  template <class T>
  class intrusive_ptr {
    T * p;

  public:

    using element_type = T;

    template <class U>
    friend class intrusive_ptr;

    BAD(hd,inline,noalias) constexpr
    intrusive_ptr() noexcept
    : p() {}

    BAD(hd,inline)
    intrusive_ptr(BAD(noescape) intrusive_ptr && rhs) noexcept
    : p(rhs.p) {
      rhs.p = nullptr;
    }

    BAD(hd,inline)
    intrusive_ptr(T * p, bool add_ref = true) noexcept
    : p(p) {
      if (p != nullptr && add_ref) acquire(p);
    }

    BAD(hd,inline)
    intrusive_ptr(intrusive_ptr const & rhs) noexcept
    : p(rhs.p) {
      if (p != nullptr) acquire(p);
    }

    template <class Y>
    BAD(hd,inline)
    intrusive_ptr(intrusive_ptr<Y> const & rhs) noexcept
    : p(static_cast<T*>(rhs.p)) {
      if (p != nullptr) acquire(p);
    }

    BAD(hd,inline)
    ~intrusive_ptr() {
      if (p != nullptr) release(p);
    }

    BAD(hd,inline)
    intrusive_ptr & operator = (BAD(noescape) intrusive_ptr && rhs) noexcept {
      intrusive_ptr(std::move(rhs)).swap(*this);
      return *this;
    }

    BAD(hd,inline)
    intrusive_ptr & operator = (BAD(noescape) intrusive_ptr const & rhs) noexcept {
      intrusive_ptr(rhs).swap(*this);
      return *this;
    }

    template <class Y>
    BAD(hd,inline)
    intrusive_ptr & operator = (BAD(noescape) intrusive_ptr<Y> const & rhs) noexcept {
      intrusive_ptr(rhs).swap(*this);
      return *this;
    }

    BAD(hd,inline)
    intrusive_ptr & operator = (T * rhs) noexcept {
      intrusive_ptr(rhs).swap(*this);
      return *this;
    }

    BAD(reinitializes,hd,inline)
    void reset() noexcept {
      intrusive_ptr().swap(*this);
    }

    BAD(reinitializes,hd,inline)
    void reset(T * rhs) noexcept {
      intrusive_ptr(rhs).swap(*this);
    }

    BAD(reinitializes,hd,inline)
    void reset(T * rhs, bool add_ref) noexcept {
      intrusive_ptr(rhs, add_ref).swap(*this);
    }

    BAD(hd,inline,pure) constexpr
    T & operator*() const noexcept {
      assert(p != nullptr);
      return *p;
    }

    BAD(hd,inline,pure) constexpr
    T * operator->() const noexcept {
      assert(p != nullptr);
      return p;
    }

    BAD(hd,inline,pure) constexpr
    T * get() const noexcept {
      return p;
    }

    BAD(hd,inline,noalias)
    T * detach() noexcept {
      T * r = p;
      p = nullptr;
      return r;
    }

    BAD(hd,inline,pure) constexpr
    operator bool () const noexcept {
      return p != nullptr;
    }

    BAD(hd,inline,pure) constexpr
    bool operator ! () const noexcept {
      return p == nullptr;
    }

    BAD(hd,inline,noalias)
    void swap(
      BAD(noescape) intrusive_ptr & rhs
    ) noexcept {
      T * t = p;
      p = rhs.p;
      rhs.p = t;
    }
  };

  // CTAD guides
  template <class T> intrusive_ptr(T * p) -> intrusive_ptr<T>;
  template <class T> intrusive_ptr(intrusive_ptr<T> const & p) -> intrusive_ptr<T>;
  template <class T> intrusive_ptr(intrusive_ptr<T> && p) -> intrusive_ptr<T>;

  template <class T, class U>
  BAD(hd,nodiscard,inline) constexpr
  bool operator==(
    BAD(noescape) intrusive_ptr<T> const & a,
    BAD(noescape) intrusive_ptr<U> const & b
  ) noexcept {
    return a.get() == b.get();
  }

  template <class T, class U>
  BAD(hd,nodiscard,inline) constexpr
  bool operator!=(
    BAD(noescape) intrusive_ptr<T> const & a,
    BAD(noescape) intrusive_ptr<U> const & b
  ) noexcept {
    return a.get() != b.get();
  }

  template <class T>
  BAD(hd,nodiscard,inline) constexpr
  bool operator==(
    BAD(noescape) intrusive_ptr<T> const & a,
    BAD(noescape) T * b
  ) noexcept {
    return a.get() == b;
  }

  template <class T>
  BAD(hd,nodiscard,inline) constexpr
  bool operator!=(
    BAD(noescape) intrusive_ptr<T> const & a,
    BAD(noescape) T * b
  ) noexcept {
    return a.get() != b;
  }

  template <class T>
  BAD(hd,nodiscard,inline) constexpr
  bool operator==(
    BAD(noescape) T * a,
    BAD(noescape) intrusive_ptr<T> const & b
  ) noexcept {
    return a == b.get();
  }

  template <class T>
  BAD(hd,nodiscard,inline) constexpr
  bool operator!=(
    BAD(noescape) T * a,
    BAD(noescape) intrusive_ptr<T> const & b
  ) noexcept {
    return a != b.get();
  }

  template <class T, class U>
  BAD(hd,nodiscard,inline) constexpr
  bool operator<(
    BAD(noescape) intrusive_ptr<T> const & a,
    BAD(noescape) intrusive_ptr<U> const & b
  ) noexcept {
    return a.get() < b.get();
  }

  template <class T, class U>
  BAD(hd,nodiscard,inline) constexpr
  bool operator>(
    BAD(noescape) intrusive_ptr<T> const & a,
    BAD(noescape) intrusive_ptr<U> const & b
  ) noexcept {
    return a.get() > b.get();
  }

  template <class T, class U>
  BAD(hd,nodiscard,inline) constexpr
  bool operator<=(
    BAD(noescape) intrusive_ptr<T> const & a,
    BAD(noescape) intrusive_ptr<U> const & b
  ) noexcept {
    return a.get() <= b.get();
  }

  template <class T, class U>
  BAD(hd,nodiscard,inline) constexpr
  bool operator>=(
    BAD(noescape) intrusive_ptr<T> const & a,
    BAD(noescape) intrusive_ptr<U> const & b
  ) noexcept {
    return a.get() >= b.get();
  }

  template <class T>
  BAD(hd,inline)
  void swap(
    BAD(noescape) intrusive_ptr<T> & a,
    BAD(noescape) intrusive_ptr<T> & b
  ) noexcept {
    a.swap(b);
  }

  template <class T>
  BAD(hd,nodiscard,inline) constexpr
  T * get_pointer(intrusive_ptr<T> const & p) noexcept {
    return p.get();
  }

  template <class T, class U>
  BAD(hd,nodiscard,inline) constexpr
  intrusive_ptr<T> static_pointer_cast(
    intrusive_ptr<U> const & p
  ) noexcept {
    return static_cast<T *>(p.get());
  }

  template <class T, class U>
  BAD(hd,nodiscard,inline) constexpr
  intrusive_ptr<T> const_pointer_cast(
    intrusive_ptr<U> const & p
  ) noexcept {
    return const_cast<T *>(p.get());
  }

  template <class T, class U>
  BAD(hd,nodiscard,inline)
  intrusive_ptr<T> dynamic_pointer_cast(
    intrusive_ptr<U> const & p
  ) noexcept {
    return dynamic_cast<T *>(p.get());
  }

  template <class E, class T, class Y>
  std::basic_ostream<E, T> & operator<<(
    std::basic_ostream<E, T> & os,
    intrusive_ptr<Y> const & p
  ) {
    os << p.get();
    return os;
  }
}

namespace std {
  template <class T>
  struct BAD(empty_bases) hash<bad::intrusive_ptr<T>> {
    BAD(hd,nodiscard,inline)
    std::size_t operator()(
      BAD(noescape) bad::intrusive_ptr<T> const & p
    ) const noexcept {
      return std::hash<T*>{}(p.get());
    }
  };
}

namespace bad {
  using namespace bad::memory;
}

#endif
