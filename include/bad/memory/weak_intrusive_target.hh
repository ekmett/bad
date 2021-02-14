#ifndef BAD_MEMORY_WEAK_INTRUSIVE_TARGET_HH
#define BAD_MEMORY_WEAK_INTRUSIVE_TARGET_HH

#include <mutex>

#include "bad/common.hh"
#include "bad/memory/intrusive_ptr.hh"
#include "bad/memory/intrusive_target.hh"
#include "bad/memory/weak_intrusive_ptr.hh"

namespace bad::memory {

  template <class T, class Policy = atomic_policy>
  struct weak_intrusive_target;
  namespace detail {
    /// \ingroup intrusive_group
    /// \meta
    template <class T, class Policy>
    struct weak final : intrusive_target<weak<T,Policy>, Policy> {
      using policy = Policy;
    private:
      using policy_mutex = typename policy::mutex_type;
  
      mutable policy_mutex mutex;
      mutable T * actual; 

      BAD(hd,inline)
      weak(T * p) noexcept : mutex(), actual(p) {}
  
    public:
      BAD(hd,inline)
      intrusive_ptr<T> reclaim() const noexcept {
        std::shared_lock lock(mutex);
        return actual; // bumps ref count and revives if not null
      }
  
    private:
      friend weak_intrusive_target<T,Policy>;

      BAD(hd,inline)
      bool renounce(typename policy::type & s) noexcept {
        std::unique_lock lock(mutex);
        if (!policy::load(s)) { // nobody raced in and resurrected us
          actual = nullptr;
          return true;
        } 
      }
    };
  }

  /// \ingroup intrusive_group
  template <class T, class Policy>
  struct weak_intrusive_target {
    using policy = Policy;

  private:
    intrusive_ptr<detail::weak<T,Policy>> this_;

    friend weak_intrusive_ptr<T>;

    BAD(hd,inline)
    detail::weak<T,Policy> * make_weak() const noexcept {
      return this_.get();
    }

    mutable typename policy::type ref_count;

  public:
    BAD(hd,inline)
    weak_intrusive_target() noexcept 
    : ref_count(0)
    , this_(new detail::weak<T,Policy>(this)) {}

    BAD(hd,inline)
    weak_intrusive_target(weak_intrusive_target const & rhs) noexcept
    : ref_count(0)
    , this_(new detail::weak<T,Policy>(this)) {}

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
          delete static_cast<T const *>(this);
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

  /// \ingroup intrusive_group
  template <class T>
  using thread_unsafe_weak_intrusive_target = weak_intrusive_target<T, thread_unsafe_policy>;

  /// \ingroup intrusive_group
  template <class T, class Policy>
  BAD(hd,inline)
  void acquire(
    BAD(noescape) weak_intrusive_target<T,Policy> * x
  ) noexcept {
    assert(x);
    x->acquire();
  }

  /// \ingroup intrusive_group
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
