#ifndef BAD_DISJOINT_SETS_HH
#define BAD_DISJOINT_SETS_HH

#include "bad/common.hh"
#include "bad/attributes.hh"
#include "bad/memory.hh"

// TODO: atomics or locks for better safety in concurrent situations

/// \file
/// better disjoint set forests
///
/// \defgroup disjoint_sets_group disjoint sets
/// better disjoint set forests

/// \namespace bad
/// \private
namespace bad {
  /// \namespace bad::disjoint_sets
  /// \ref disjoint_sets_group "disjoint sets" internals, import bad::disjoint_sets::api
  /// \ingroup disjoint_sets_group
  namespace disjoint_sets {
    /// \namespace bad::disjoint_sets::common
    /// \ingroup disjoint_sets_group
    /// re-exported by \ref bad and bad::disjoint_sets::api
    namespace common {}
    /// \namespace bad::errors::api
    /// \ingroup errors_group
    /// See \ref errors_group "errors" for a complete listing.
    namespace api { using namespace common; }
    using namespace api;
  }
  using namespace disjoint_sets::common;
}

namespace bad::disjoint_sets {
  using namespace bad::memory::api;

  template <class T>
  struct dso;

  namespace common {
    template <class T>
    using ds = rc<dso<T>>;
  }

  // T should support T += T
  template <class T>
  struct root {
    T data;
    mutable size_t rank;

    template <typename... Args>
    BAD(hd,inline)
    explicit root(Args...args)
    : data(std::forward(args)...), rank(0) {}
  };

  template <class T>
  struct link {
    mutable ds<T> parent; // may be mutably forwarded by find, not user visible as such

    template <typename... Args>
    BAD(hd,inline)
    explicit link(Args...args)
    : parent(std::forward(args)...) {}
  };

  template <class T>
  struct dso : counted<dso<T>> {
    using entry_type = std::variant<root<T>,link<T>>;
    entry_type entry;

    template <typename... Args>
    BAD(hd,inline)
    explicit dso(Args...args)
    : counted<dso<T>>()
    , entry(std::forward(args)...) {}

    BAD(hd,inline,flatten)
    ds<T> parent() noexcept {
      return std::visit([](auto x) {
        if constexpr (std::is_same_v<decltype(x),link<T>>) {
          return x.parent;
        } else {
          return *this;
        }
      },entry);
    }

  protected:
    template <class ... Args>
    BAD(hd,inline,flatten)
    void set_parent(Args ... args) noexcept {
      entry.emplace<link<T>>(args...);
    }
  };

  template <class T, class...Args>
  BAD(hd,inline,flatten)
  ds<T> make_set(Args... args) noexcept {
    // allocate a fresh root
    return new dso<T>(std::in_place_index<0>, std::forward(args)...);
  }

  // with (mutable) path-halving
  template <class T>
  BAD(hd,inline,flatten)
  ds<T> find(ds<T> const & start) noexcept const {
    ds<T> x = start;
    while (x->parent() != x) {
      x->set_parent(x->parent()->parent());
      x = x->parent();
    }
    return x;
  }

  template <class T>
  BAD(hd,inline,flatten)
  ds<T> union(ds<T> const & x, ds<T> const & y) {
    auto xp = find(x);
    auto yp = find(y);
    auto xr = std::get<0>(xp.entry);
    auto yr = std::get<0>(yp.entry);

    int dr = xr.rank - yr.rank;

    if (dr <= 0) {
      xr.data += yr.data;
      yp.set_parent(xp);
      if (dr == 0) ++xr.rank;
      return xp;
    } else {
      yr.data += xr.data;
      xp.set_parent(yp);
      return yp;
    }
  }

  // result is valid until you do another union
  template <class T>
  BAD(hd,inline,flatten)
  T & measure(ds<T> const & x) {
    return std::get<0>(find(x)->entry).data;
  }
}

#endif
