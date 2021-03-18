#ifndef BAD_POINTERS_INTRUSIVE_POLICY_HH
#define BAD_POINTERS_INTRUSIVE_POLICY_HH

#include <atomic>
#include <shared_mutex>
#include "bad/common.hh"
#include "bad/concurrency.hh"

namespace bad::pointers {

  // TODO: incorporate an allocator into the policy so we can free on the gpu, etc.

  /// \ingroup intrusive_group
  /// use when you aren't worried about multi-threaded access
  struct BAD(empty_bases) thread_unsafe_policy {
    using mutex = null_mutex;
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

  /// \ingroup intrusive_group
  /// use when you are worried about multi-threaded access and need atomic reference count updates
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
}

namespace bad {
  using namespace bad::pointers;
}

#endif
