#ifndef BAD_TREES_DISJOINT_SETS_HH
#define BAD_TREES_DISJOINT_SETS_HH

#include <variant>
#include <utility>

#include "bad/common.hh"
#include "bad/attributes.hh"
#include "bad/memory.hh"

// TODO: atomics and locks for better safety in concurrent situations

/// \file
/// disjoint set forests
///
/// \defgroup disjoint_sets_group disjoint sets
/// \ingroup trees_group
/// disjoint set forests

namespace bad::trees {
  namespace detail {
    /// \private
    /// \ingroup disjoint_sets_group
    template <class T>
    struct dso;
  
    /// \private
    /// \ingroup disjoint_sets_group
    /// T should support T += T
    template <class T>
    struct root final {
      T data;
      mutable size_t rank;
  
      template <typename... Args>
      BAD(hd,inline) constexpr
      explicit root(T const & rhs) noexcept
      : data(rhs)
      , rank(0) {}
    };
  }

  /// \ingroup disjoint_sets_group
  /// disjoint sets with union-find
  ///
  /// The current implementation uses Tarjan-style path-halving and union-by-rank
  /// carried values are merged with `T += T`
  template <class T>
  struct disjoint final {
  private:
    using dso = detail::dso<T>;
    using root = detail::root<T>;
    mutable intrusive_ptr<dso> p;

  public:
    /// constructs a fresh disjoint set initialized with T's default constructor
    BAD(hd,inline) constexpr
    disjoint() noexcept;

    BAD(hd,inline) constexpr
    disjoint(disjoint const & rhs) noexcept
    : p(rhs.p) {}

    BAD(hd,inline) constexpr
    disjoint(disjoint && rhs) noexcept
    : p(std::move(rhs.p)) {}

  private:
    BAD(hd,inline) constexpr
    disjoint(dso * rhs) noexcept
    : p(rhs) {
      assert(rhs != nullptr);
    }

    friend dso;

  public:
    BAD(hd,inline) constexpr
    disjoint(T const &) noexcept;

    BAD(hd,inline)
    disjoint & operator = (disjoint const & rhs) noexcept {
      p = rhs.p;
      return *this;
    }

    BAD(hd,inline)
    disjoint & operator = (disjoint && rhs) noexcept {
      p = rhs.p;
      return *this;
    }

  private:
    BAD(hd,inline) constexpr
    root & find() const noexcept;

  public:
    /// merge two disjoint sets
    BAD(hd,inline)
    disjoint & operator | (disjoint &) noexcept;

    /// the returned reference remains valid at least until a `merge` involves this disjoint set.
    BAD(hd,inline) constexpr
    T & value() noexcept {
      return find().data;
    }

    /// the returned reference remains valid at least until a `merge` involves this disjoint set.
    BAD(hd,inline) constexpr
    T const & value() const noexcept {
      return find().data;
    }

    /// do these two disjoint sets share the same root?
    BAD(hd,inline) constexpr
    friend bool operator == (
      BAD(noescape) disjoint const & lhs,
      BAD(noescape) disjoint const & rhs
    ) noexcept {
      return &lhs.find() == &rhs.find();
    }

    /// do these two disjoint sets not share the same root?
    BAD(hd,inline) constexpr
    friend bool operator != (
      BAD(noescape) disjoint const & lhs,
      BAD(noescape) disjoint const & rhs
    ) noexcept {
      return &lhs.find() != &rhs.find();
    }

  private:
    BAD(hd,inline) constexpr
    dso * operator ->() const noexcept {
      auto result = p.get();
      assert(result != nullptr);
      return result;
    }

    BAD(hd,inline) constexpr
    dso & operator *() const noexcept {
      auto result = p.get();
      assert(result != nullptr);
      return *result;
    }

    BAD(hd,inline) constexpr
    disjoint<T> parent() const noexcept;

    BAD(hd,inline) constexpr
    void set_parent(disjoint<T> const &) const noexcept;
  };

  template <class T>
  disjoint(disjoint<T> const &) -> disjoint<T>;

  template <class T>
  disjoint(disjoint<T> &&) -> disjoint<T>;

  template <class T>
  disjoint(T) -> disjoint<T>;

  namespace detail {

    /// \private
    /// \ingroup disjoint_sets_group
    template <class T>
    struct child final {
      mutable disjoint<T> parent; // may be mutably forwarded by find, not user visible as such
  
      template <typename... Args>
      BAD(hd,inline)
      explicit child(Args...args) noexcept
      : parent(std::forward<Args>(args)...) {}
    };
  
    /// \private
    /// \ingroup disjoint_sets_group
    ///
    /// The selected policy for intrusive_target<dso<T>> means that these are _not_ thread safe.
    template <class T>
    struct dso final : intrusive_target<dso<T>> {
      friend disjoint<T>;
      /// TODO: eventually carry a shared mutex to control access to entry
  
      using entry_type = std::variant<root<T>,child<T>>;
      entry_type entry;
  
      template <class... Args>
      BAD(hd,inline)
      explicit dso(Args&&... args) noexcept
      : intrusive_target<dso<T>>()
      , entry(std::forward<Args>(args)...) {}
  
      BAD(hd,inline,flatten)
      disjoint<T> parent() noexcept {
        dso * self = this;
        assert(self != nullptr);
        return std::visit([self](auto v) {
          if constexpr (std::is_same_v<decltype(v),child<T>>) {
            return v.parent;
          } else {
            return disjoint<T>(self);
          }
        },entry);
      }
  
      template <class ... Args>
      BAD(hd,inline,flatten)
      void set_parent(Args&& ... args) noexcept {
        entry.template emplace<child<T>>(std::forward<Args>(args)...);
      }
    };
  }

  template <class T> constexpr
  disjoint<T> disjoint<T>::parent() const noexcept {
    return disjoint(p->parent());
  }

  template <class T> constexpr
  void disjoint<T>::set_parent(disjoint<T> const & rhs) const noexcept {
    p->set_parent(rhs);
  }

  template <class T> constexpr
  disjoint<T>::disjoint() noexcept : p(new detail::dso<T>()) {}

  template <class T> constexpr
  disjoint<T>::disjoint(T const & t) noexcept
  : p (new detail::dso<T>(std::in_place_index_t<0>(),t)) {}

  template <class T> constexpr
  detail::root<T> & disjoint<T>::find() const noexcept {
    while (p != parent().p) {
       set_parent(parent()->parent());
       p = parent().p;
    }
    return std::get<0>(p->entry);
  }

  template <class T>
  disjoint<T> & disjoint<T>::operator |(disjoint<T> & rhs) noexcept {
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

  /// \ingroup disjoint_sets_group
  template <class... Args>
  auto merge(Args...args) noexcept {
    return (args | ...);
  }
}

namespace bad {
  using namespace bad::trees;
}

#endif
