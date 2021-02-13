#ifndef BAD_MEMORY_INTRUSIVE_TARGET_HH
#define BAD_MEMORY_INTRUSIVE_TARGET_HH

#include "bad/common.hh"

namespace bad::memory {

  // TODO: incorporate an allocator into the policy so we can free on the gpu, etc.

  struct thread_unsafe_policy {
    using type = size_t;

    BAD(hd,inline,const)
    static size_t load(type s) {
      return s;
    }

    BAD(hd,inline,noalias)
    static void inc(
      BAD(noescape) type & s
    ) noexcept {
      ++s;
    }

    BAD(hd,inline,noalias)
    static size_t dec(
      BAD(noescape) type & s
    ) noexcept {
      return --s;
    }
  };

  // this will probably have to be hacked up to work with cuda as std::atomic and cuda::atomic are separate beasts
  struct atomic_policy {
    using type = std::atomic_size_t;

    BAD(hd,inline,const)
    static size_t load(type const & s) {
      return s.load();
    }

    BAD(hd,inline,noalias)
    static void inc(
      BAD(noescape) type & s
    ) noexcept {
      ++s;
    }

    BAD(hd,inline,noalias)
    static size_t dec(
      BAD(noescape) type & s
    ) noexcept {
      return --s;
    }
  };

  template <class B, class Policy = thread_unsafe_policy>
  struct intrusive_target {
    using policy = Policy;

  private:
    using ref_count_type = typename policy::type;
    mutable ref_count_type ref_count;

  public:
    BAD(hd,inline)
    intrusive_target() noexcept : ref_count(0) {}

    BAD(hd,inline)
    intrusive_target(BAD(maybe_unused) intrusive_target const & rhs) noexcept : ref_count(0) {}

    BAD(hd,inline,const)
    intrusive_target & operator = (BAD(maybe_unused) intrusive_target const & rhs) noexcept {
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
        delete static_cast<B const *>(this);
    }
  };

  template <class B>
  using atomically_intrusive_target = intrusive_target<B,atomic_policy>;

  template <class B, class Policy>
  BAD(hd,inline)
  void acquire(intrusive_target<B,Policy> const * rhs) noexcept {
    assert(rhs != nullptr);
    rhs->acquire();
  }

  template <class B, class Policy>
  BAD(hd,inline)
  void release(intrusive_target<B,Policy> const * rhs) noexcept {
    assert(rhs != nullptr);
    rhs->release();
  }
}

namespace bad {
  using namespace bad::memory;

}

#endif
