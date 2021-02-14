#ifndef BAD_CONCURRENCY_HH
#define BAD_CONCURRENCY_HH

/// \file
/// \brief concurrency primitives
/// \author Edward Kmett

/// \defgroup concurrency_group concurrency
/// \brief concurrency primitives

namespace bad::concurrency {
  /// \ingroup concurrency_group
  /// a trivial mutex
  struct BAD(empty_bases) null_mutex {
    BAD(hd,inline,const) constexpr
    void lock() const noexcept {}
    BAD(hd,inline,const) constexpr
    void unlock() const noexcept {}
    BAD(hd,inline,const) constexpr
    bool try_lock() const noexcept { return true; }
    BAD(hd,inline,const) constexpr
    void lock_shared() const noexcept {}
    BAD(hd,inline,const) constexpr
    void unlock_shared() const noexcept {}
    BAD(hd,inline,const) constexpr
    bool try_lock_shared() const noexcept { return true; }
  };
}

namespace bad {
  using namespace bad::concurrency;
}

#endif
