#ifndef BAD_DISJOINT_SETS_HH
#define BAD_DISJOINT_SETS_HH

#include <variant>
#include <utility>

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

  // T should support T += T
  template <class T>
  struct root {
    T data;
    mutable size_t rank;

    template <typename... Args>
    BAD(hd,inline)
    explicit root(T const & rhs)
    : data(rhs)
    , rank(0) {}
  };

  namespace common {

    template <class T>
    struct ds {
      mutable rc<dso<T>> p;

      BAD(hd,inline)
      ds() noexcept;

      BAD(hd,inline)
      ds(ds const & rhs) noexcept
      : p(rhs.p) {}

      BAD(hd,inline)
      ds(ds && rhs) noexcept
      : p(std::move(rhs.p)) {}

      // protected
      BAD(hd,inline)
      ds(dso<T> * rhs) noexcept
      : p(rhs) {
        assert(rhs != nullptr);
      }

      // template <class...Args>
      // BAD(hd,inline)
      // ds(Args&&...) noexcept;

      BAD(hd,inline)
      ds(T const &) noexcept;

      BAD(hd,inline)
      ds & operator = (ds const & rhs) noexcept {
        p = rhs.p;
        return *this;
      }

      BAD(hd,inline)
      ds & operator = (ds && rhs) noexcept {
        p = rhs.p;
        return *this;
      }

    // protected:
      BAD(hd,inline)
      root<T> & find() const noexcept;

    // public:
      BAD(hd,inline)
      ds & operator | (ds &) noexcept;

      BAD(hd,inline)
      T & value() noexcept {
        return find().data;
      }

      BAD(hd,inline)
      T const & value() const noexcept {
        return find().data;
      }

      BAD(hd,inline)
      friend bool operator == (
        BAD(noescape) ds const & lhs,
        BAD(noescape) ds const & rhs
      ) noexcept {
        return &lhs.find() == &rhs.find();
      }

      BAD(hd,inline)
      friend bool operator != (
        BAD(noescape) ds const & lhs,
        BAD(noescape) ds const & rhs
      ) noexcept {
        return &lhs.find() != &rhs.find();
      }

      BAD(hd,inline)
      dso<T> * operator ->() const noexcept {
        auto result = p.get();
        assert(result != nullptr);
        return result;
      }

      BAD(hd,inline)
      dso<T> & operator *() const noexcept {
        auto result = p.get();
        assert(result != nullptr);
        return *result;
      }

    // protected
      BAD(hd,inline)
      ds<T> parent() const noexcept;

      BAD(hd,inline)
      void set_parent(ds<T> const &) const noexcept;
    };

    template <class T>
    ds(ds<T> const &) -> ds<T>;

    template <class T>
    ds(ds<T> &&) -> ds<T>;

    template <class T>
    ds(T) -> ds<T>;
  }

  template <class T>
  struct link {
    mutable ds<T> parent; // may be mutably forwarded by find, not user visible as such

    template <typename... Args>
    BAD(hd,inline)
    explicit link(Args...args)
    : parent(std::forward<Args>(args)...) {}
  };

  template <class T>
  struct dso : counted<dso<T>> {
    friend ds<T>;

    using entry_type = std::variant<root<T>,link<T>>;
    entry_type entry;

    template <class... Args>
    BAD(hd,inline)
    explicit dso(Args&&... args)
    : counted<dso<T>>()
    , entry(std::forward<Args>(args)...) {}

    BAD(hd,inline,flatten)
    ds<T> parent() noexcept {
      dso * self = this;
      assert(self != nullptr);
      return std::visit([self](auto root_or_link) {
        if constexpr (std::is_same_v<decltype(root_or_link),link<T>>) {
          return root_or_link.parent;
        } else {
          return ds<T>(self);
        }
      },entry);
    }

    template <class ... Args>
    BAD(hd,inline,flatten)
    void set_parent(Args&& ... args) noexcept {
      entry.template emplace<link<T>>(std::forward<Args>(args)...);
    }
  };

  template <class T>
  common::ds<T> common::ds<T>::parent() const noexcept {
    return ds(p->parent());
  }

  template <class T>
  void common::ds<T>::set_parent(ds<T> const & rhs) const noexcept {
    p->set_parent(rhs);
  }

  template <class T>
  common::ds<T>::ds() noexcept : p(new dso<T>()) {}

  // template <class T>
  // template <class...Args>
  // common::ds<T>::ds(Args&&...args) noexcept
  // : p (new dso<T>(std::in_place_index_t<0>(),std::forward<Args>(args)...)) {}

  template <class T>
  common::ds<T>::ds(T const & t) noexcept
  : p (new dso<T>(std::in_place_index_t<0>(),t)) {}


  template <class T>
  root<T> & common::ds<T>::find() const noexcept {
    while (p != parent().p) {
       set_parent(parent()->parent());
       p = parent().p;
    }
    return std::get<0>(p->entry);
  }

  template <class T>
  ds<T> & common::ds<T>::operator |(ds<T> & rhs) noexcept {
    auto & xr = find();
    auto & yr = rhs.find();

    ptrdiff_t dr = xr.rank - yr.rank;

    if (dr >= 0) {
      xr.data += yr.data;
      rhs.p->set_parent(*this);
      // yr is now invalid
      if (dr == 0) ++xr.rank;
      return *this;
    } else {
      yr.data += xr.data;
      p->set_parent(rhs);
      return rhs;
    }
  }

  namespace common {
    template <class T, class... Args>
    ds<T> & merge(ds<T> arg, Args...args) {
      return (arg | ... | args);
    }

    template <class T>
    ds<T> & merge() {
      return ds<T>();
    }
  }
}

#endif