#ifndef BAD_CONCURRENCY_HH
#define BAD_CONCURRENCY_HH

#include "bad/attributes.hh"

/// \file
/// \brief concurrency primitives
/// \author Edward Kmett

/// \defgroup concurrency_group concurrency
/// \brief concurrency primitives

namespace bad::concurrency {

  /// \ingroup concurrency_group
  /// a trivial mutex that lets you get away with anything
  struct BAD(empty_bases) null_mutex {

    // not copyable, not movable

    // DefaultConstructible

    BAD(hd,inline) constexpr
    null_mutex() noexcept {}

    BAD(hd,inline,const) constexpr
    null_mutex & operator = (
      BAD(maybe_unused) const null_mutex &
    ) noexcept {
      return *this;
    }

    // Destructible

    BAD(hd,inline,const)
    ~null_mutex() noexcept {}

    // BasicLockable

    BAD(hd,inline,const) constexpr
    void lock() const noexcept {}

    BAD(hd,inline,const) constexpr
    void unlock() const noexcept {}

    // Lockable

    BAD(hd,inline,const) constexpr
    bool try_lock() const noexcept {
      return true;
    }

    // TimedLockable

    template <class Rep, class Period>
    BAD(hd,inline,const) constexpr
    bool try_lock_for(
      BAD(maybe_unused) const std::chrono::duration<Rep,Period> &
    ) const noexcept {
      return true;
    }

    template <class Clock, class Duration>
    BAD(hd,inline,const) constexpr
    bool try_lock_until(
      BAD(maybe_unused) const std::chrono::time_point<Clock,Duration> &
    ) const noexcept {
      return true;
    }

    // SharedMutex

    BAD(hd,inline,const) constexpr
    void lock_shared() const noexcept {}

    BAD(hd,inline,const) constexpr
    void unlock_shared() const noexcept {}

    BAD(hd,inline,const) constexpr
    bool try_lock_shared() const noexcept {
      return true;
    }

    BAD(hd,inline,const) constexpr
    void unlock_and_lock_sharable() const noexcept {}

    // TimedSharedMutex

    template <class Rep, class Period>
    BAD(hd,inline,const) constexpr
    bool try_lock_shared_for(
      BAD(maybe_unused) std::chrono::duration<Rep,Period> const &
    ) const noexcept {
      return true;
    }

    template <class Clock, class Duration>
    BAD(hd,inline,const) constexpr
    bool try_lock_shared_until(
      BAD(maybe_unused) std::chrono::time_point<Clock,Duration> const &
    ) const noexcept {
      return true;
    }

    BAD(hd,inline,const) constexpr
    bool try_unlock_sharable_and_lock() const noexcept {
      return true;
    }

    // UpgradableMutex

    BAD(hd,inline,const) constexpr
    void lock_upgradable() const noexcept {}

    BAD(hd,inline,const) constexpr
    bool unlock_upgradable() const noexcept {
      return true;
    }

    BAD(hd,inline,const) constexpr
    bool try_lock_upgradable() const noexcept {
      return true;
    }

    // TimedUpgradableMutex

    template <class Rep, class Period>
    BAD(hd,inline,const) constexpr
    bool try_lock_upgradable_for(
      BAD(maybe_unused) std::chrono::duration<Rep,Period> const & duration
    ) const noexcept {
      return true;
    }

    template <class Clock, class Duration>
    BAD(hd,inline,const) constexpr
    bool try_lock_upgradable_until(
      BAD(maybe_unused) std::chrono::time_point<Clock,Duration> const & timeout_time
    ) const noexcept {
      return true;
    }

    BAD(hd,inline,const) constexpr
    void unlock_and_lock_upgradable() const noexcept {}

    BAD(hd,inline,const) constexpr
    void unlock_upgradable_and_lock() const noexcept {}

    BAD(hd,inline,const) constexpr
    bool try_unlock_upgradable_and_lock() const noexcept {
      return true;
    }

    BAD(hd,inline,const) constexpr
    void unlock_upgradable_and_lock_sharable() const noexcept {}

    BAD(hd,inline,const) constexpr
    bool try_unlock_sharable_and_lock_upgradable() const noexcept {
      return true;
    }

    template <class Rep, class Period>
    BAD(hd,inline,const) constexpr
    bool try_unlock_upgradable_and_lock_for(
      BAD(maybe_unused) std::chrono::duration<Rep,Period> const & duration
    ) const noexcept {
      return true;
    }

    template <class Clock, class Duration>
    BAD(hd,inline,const) constexpr
    bool try_unlock_upgradable_and_lock_until(
      BAD(maybe_unused) std::chrono::time_point<Clock,Duration> const & timeout_time
    ) const noexcept {
      return true;
    }
  };
}

namespace bad {
  using namespace bad::concurrency;
}

#endif
