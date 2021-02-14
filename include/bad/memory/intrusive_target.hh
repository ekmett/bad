#ifndef BAD_MEMORY_INTRUSIVE_TARGET_HH
#define BAD_MEMORY_INTRUSIVE_TARGET_HH

#include <shared_mutex>
#include "bad/common.hh"

namespace bad::memory {

  namespace detail {
    struct BAD(empty_bases) null_mutex {
      void lock() {}
      void unlock() {}
      bool try_lock() { return true; }
      void lock_shared() {}
      void unlock_shared() {}
      bool try_lock_shared() { return true; }
    };
  }

  // TODO: incorporate an allocator into the policy so we can free on the gpu, etc.

  struct BAD(empty_bases) thread_unsafe_policy {
    using mutex = detail::null_mutex;
    using counter = size_t;

    BAD(hd,inline,const)
    static size_t load(counter s) {
      return s;
    }

    BAD(hd,inline,noalias)
    static void inc(
      BAD(noescape) counter & s
    ) noexcept {
      ++s;
    }

    BAD(hd,inline,noalias)
    static size_t dec(
      BAD(noescape) counter & s
    ) noexcept {
      return --s;
    }
  };

  // this will probably have to be hacked up to work with cuda as std::atomic and cuda::atomic are separate beasts
  struct BAD(empty_bases) atomic_policy {
    using shared_mutex = std::shared_mutex; // used by weak_intrusive_target;
    using counter = std::atomic_size_t;

    BAD(hd,inline,const)
    static size_t load(counter const & s) {
      return s.load();
    }

    BAD(hd,inline,noalias)
    static void inc(
      BAD(noescape) counter & s
    ) noexcept {
      ++s;
    }

    BAD(hd,inline,noalias)
    static size_t dec(
      BAD(noescape) counter & s
    ) noexcept {
      return --s;
    }
  };

  template <class B, class Policy = atomic_policy>
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
        delete static_cast<B const *>(this);
    }
  };

  template <class B>
  using thread_unsafe_intrusive_target = intrusive_target<B,thread_unsafe_policy>;

  template <class B, class Policy>
  BAD(hd,inline)
  void acquire(intrusive_target<B,Policy> const * x) noexcept {
    assert(x);
    x->acquire();
  }

  template <class B, class Policy>
  BAD(hd,inline)
  void release(intrusive_target<B,Policy> const * x) noexcept {
    assert(x);
    x->release();
  }
}

namespace bad {
  using namespace bad::memory;

}

#endif
