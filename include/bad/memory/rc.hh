#ifndef BAD_MEMORY_RC_HH
#define BAD_MEMORY_RC_HH

#include <iosfwd>

#include "bad/attributes.hh"
#include "bad/memory/namespace.hh"

namespace bad::memory::api {

  // reference counted intrusive pointers
  template <class T>
  class rc {
    T * p;

  public:

    using element_type = T;

    template <class U>
    friend class rc;

    BAD(hd,inline,noalias) constexpr
    rc() noexcept
    : p() {}

    BAD(hd,inline)
    rc(BAD(noescape) rc && rhs) noexcept
    : p(rhs.p) {
      rhs.p = nullptr;
    }

    BAD(hd,inline)
    rc(T * p, bool add_ref = true) noexcept
    : p(p) {
      if (p != nullptr && add_ref) acquire(p);
    }

    BAD(hd,inline)
    rc(rc const & rhs) noexcept
    : p(rhs.p) {
      if (p != nullptr) acquire(p);
    }

    template <class Y>
    BAD(hd,inline)
    rc(rc<Y> const & rhs) noexcept
    : p(static_cast<T*>(rhs.p)) {
      if (p != nullptr) acquire(p);
    }

    BAD(hd,inline)
    ~rc() {
      if (p != nullptr) release(p);
    }

    BAD(hd,inline)
    rc & operator = (BAD(noescape) rc && rhs) noexcept {
      rc(std::move(rhs)).swap(*this);
      return *this;
    }

    BAD(hd,inline)
    rc & operator = (rc const & rhs) noexcept {
      rc(rhs).swap(*this);
      return *this;
    }

    template <class Y>
    BAD(hd,inline)
    rc & operator = (rc<Y> const & rhs) noexcept {
      rc(rhs).swap(*this);
      return *this;
    }

    BAD(hd,inline)
    rc & operator = (T * rhs) noexcept {
      rc(rhs).swap(*this);
      return *this;
    }

    BAD(reinitializes,hd,inline)
    void reset() noexcept {
      rc().swap(*this);
    }

    BAD(reinitializes,hd,inline)
    void reset(T * rhs) noexcept {
      rc(rhs).swap(*this);
    }

    BAD(reinitializes,hd,inline)
    void reset(T * rhs, bool add_ref) noexcept {
      rc(rhs, add_ref).swap(*this);
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
      BAD(noescape) rc & rhs
    ) noexcept {
      T * t = p;
      p = rhs.p;
      rhs.p = t;
    }
  };

  // CTAD guides
  template <class T> rc(T * p) -> rc<T>;
  template <class T> rc(rc<T> const & p) -> rc<T>;
  template <class T> rc(rc<T> && p) -> rc<T>;

  template <class T, class U>
  BAD(hd,nodiscard,inline) constexpr
  bool operator==(
    BAD(noescape) rc<T> const & a,
    BAD(noescape) rc<U> const & b
  ) noexcept {
    return a.get() == b.get();
  }

  template <class T, class U>
  BAD(hd,nodiscard,inline) constexpr
  bool operator!=(
    BAD(noescape) rc<T> const & a,
    BAD(noescape) rc<U> const & b
  ) noexcept {
    return a.get() != b.get();
  }

  template <class T>
  BAD(hd,nodiscard,inline) constexpr
  bool operator==(
    BAD(noescape) rc<T> const & a,
    BAD(noescape) T * b
  ) noexcept {
    return a.get() == b;
  }

  template <class T>
  BAD(hd,nodiscard,inline) constexpr
  bool operator!=(
    BAD(noescape) rc<T> const & a,
    BAD(noescape) T * b
  ) noexcept {
    return a.get() != b;
  }

  template <class T>
  BAD(hd,nodiscard,inline) constexpr
  bool operator==(
    BAD(noescape) T * a,
    BAD(noescape) rc<T> const & b
  ) noexcept {
    return a == b.get();
  }

  template <class T>
  BAD(hd,nodiscard,inline) constexpr
  bool operator!=(
    BAD(noescape) T * a,
    BAD(noescape) rc<T> const & b
  ) noexcept {
    return a != b.get();
  }

  template <class T, class U>
  BAD(hd,nodiscard,inline) constexpr
  bool operator<(
    BAD(noescape) rc<T> const & a,
    BAD(noescape) rc<U> const & b
  ) noexcept {
    return a.get() < b.get();
  }

  template <class T, class U>
  BAD(hd,nodiscard,inline) constexpr
  bool operator>(
    BAD(noescape) rc<T> const & a,
    BAD(noescape) rc<U> const & b
  ) noexcept {
    return a.get() > b.get();
  }

  template <class T, class U>
  BAD(hd,nodiscard,inline) constexpr
  bool operator<=(
    BAD(noescape) rc<T> const & a,
    BAD(noescape) rc<U> const & b
  ) noexcept {
    return a.get() <= b.get();
  }

  template <class T, class U>
  BAD(hd,nodiscard,inline) constexpr
  bool operator>=(
    BAD(noescape) rc<T> const & a,
    BAD(noescape) rc<U> const & b
  ) noexcept {
    return a.get() >= b.get();
  }

  template <class T>
  BAD(hd,inline)
  void swap(
    BAD(noescape) rc<T> & a,
    BAD(noescape) rc<T> & b
  ) noexcept {
    a.swap(b);
  }

  template <class T>
  BAD(hd,nodiscard,inline) constexpr
  T * get_pointer(rc<T> const & p) noexcept {
    return p.get();
  }

  template <class T, class U>
  BAD(hd,nodiscard,inline) constexpr
  rc<T> static_pointer_cast(
    rc<U> const & p
  ) noexcept {
    return static_cast<T *>(p.get());
  }

  template <class T, class U>
  BAD(hd,nodiscard,inline) constexpr
  rc<T> const_pointer_cast(
    rc<U> const & p
  ) noexcept {
    return const_cast<T *>(p.get());
  }

  template <class T, class U>
  BAD(hd,nodiscard,inline)
  rc<T> dynamic_pointer_cast(
    rc<U> const & p
  ) noexcept {
    return dynamic_cast<T *>(p.get());
  }

  template <class E, class T, class Y>
  std::basic_ostream<E, T> & operator<<(
    std::basic_ostream<E, T> & os,
    rc<Y> const & p
  ) {
    os << p.get();
    return os;
  }
}

namespace std {
  template <class T>
  struct BAD(empty_bases) hash<bad::memory::api::rc<T>> {
    BAD(hd,nodiscard,inline)
    std::size_t operator()(
      BAD(noescape) bad::memory::api::rc<T> const & p
    ) const noexcept {
      return std::hash<T*>{}(p.get());
    }
  };
}

#endif
