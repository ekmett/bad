#ifndef BAD_MEMORY_WEAK_HH
#define BAD_MEMORY_WEAK_HH

#include <mutex>

#include "bad/common.hh"
#include "bad/memory/intrusive_ptr.hh"
#include "bad/memory/intrusive_target.hh"

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

  // requires T extends weak_intrusive_target<T,Policy,Mutex>
  template <class T, class Policy>
  struct weak final : intrusive_target<weak<T,Policy>, Policy> {
    using policy = Policy;

    typename policy::mutex_type mutex;
    T * actual; 

    intrusive_ptr<T> reclaim() noexcept {
      std::shared_lock lock(mutex);
      return actual; // bumps ref count and revives if not null
    }

    bool renounce(typename policy::type & s) noexcept {
      std::unique_lock lock(mutex);
      if (!policy::load(s)) { // nobody raced in and resurrected us
        actual = nullptr;
        return true;
      } 
    }
  };

  template <class B, class Policy = atomic_policy>
  struct weak_intrusive_target {
    using policy = Policy;

  private:
    intrusive_ptr<weak<B,Policy>> this_;

    friend weak_intrusive_ptr<B>;

    BAD(hd,inline)
    weak<B,Policy> * make_weak() const noexcept {
      return this_.get();
    }

    mutable typename policy::type ref_count;

  public:
    BAD(hd,inline)
    weak_intrusive_target() noexcept 
    : ref_count(0)
    , this_(new weak<B,Policy>(this)) {}

    BAD(hd,inline)
    weak_intrusive_target(weak_intrusive_target const & rhs) noexcept
    : ref_count(0)
    , this_(new weak<B,Policy>(this)) {}

    BAD(hd,inline)
    weak_intrusive_target & operator = (weak_intrusive_target const & rhs) noexcept {
      return *this;
    }

    BAD(hd,inline)
    void acquire() {
      policy::inc(ref_count);
    }

    BAD(hd,inline)
    void release() {
      if (0 == policy::dec(ref_count)) {
        // make sure we don't get resurrected while we clear weak refs.
        if (this_->renounce(ref_count))
          delete static_cast<B const *>(this);
      }
    }

    BAD(hd,inline)
    size_t reference_count() {
      return policy::load(ref_count);
    }

    BAD(hd,inline)
    size_t weak_reference_count() const noexcept {
      return this_->reference_count() - 1;
    }

  protected:
    BAD(hd,inline)
    ~weak_intrusive_target() {}
  };

  template <class B>
  using thread_unsafe_weak_intrusive_target = weak_intrusive_target<B, thread_unsafe_policy>;

  template <class T, class Policy>
  BAD(hd,inline)
  void acquire(
    BAD(noescape) weak_intrusive_target<T,Policy> * x
  ) noexcept {
    assert(x);
    x->acquire();
  }

  template <class T, class Policy>
  BAD(hd,inline)
  void release(
    BAD(noescape) weak_intrusive_target<T,Policy> * x
  ) noexcept {
    assert(x);
    x->release();
  }
}

namespace bad {
  using namespace bad::memory;
}

#endif
