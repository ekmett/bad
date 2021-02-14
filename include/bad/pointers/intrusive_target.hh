#ifndef BAD_POINTERS_INTRUSIVE_TARGET_HH
#define BAD_POINTERS_INTRUSIVE_TARGET_HH

#include "bad/common.hh"
#include "bad/pointers/intrusive_policy.hh"

namespace bad::pointers {
  /// \ingroup intrusive_group
  /// a valid default target for intrusive_ptr, parameterized by the reference count maintenance policy
  template <class T, class Policy = atomic_policy>
  struct intrusive_target {
    using policy = Policy;

  private:
    using ref_counter = typename policy::counter;
    mutable ref_counter ref_count;

  protected:
    // delete via release()
    ~intrusive_target() {}

  public:
    BAD(hd,inline)
    intrusive_target() noexcept : ref_count(0) {}

    BAD(hd,inline)
    intrusive_target(
      BAD(maybe_unused) intrusive_target const & rhs
    ) noexcept : ref_count(0) {}

    BAD(hd,inline,const)
    intrusive_target & operator = (BAD(maybe_unused) intrusive_target const &) noexcept {
      return *this;
    }

    BAD(hd,inline,pure)
    size_t reference_count() noexcept {
      return policy::load(ref_count);
    }

    BAD(hd,inline)
    void acquire() const noexcept {
      policy::inc(ref_count);
    }

    BAD(hd,inline)
    void release() const noexcept {
      if (policy::dec(ref_count) == 0)
        delete static_cast<T const *>(this);
    }
  };

  /// \ingroup intrusive_group
  template <class T>
  using thread_unsafe_intrusive_target = intrusive_target<T,thread_unsafe_policy>;

  /// \ingroup intrusive_group
  template <class T, class Policy>
  BAD(hd,inline)
  void acquire(intrusive_target<T,Policy> const * x) noexcept {
    assert(x);
    x->acquire();
  }

  /// \ingroup intrusive_group
  template <class T, class Policy>
  BAD(hd,inline)
  void release(intrusive_target<T,Policy> const * x) noexcept {
    assert(x);
    x->release();
  }
}

namespace bad {
  using namespace bad::pointers;
}

#endif
