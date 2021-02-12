#ifndef BAD_DISJOINT_HH
#define BAD_DISJOINT_HH

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
/// disjoint set forests

namespace bad {
  namespace detail {
    /// \private
    /// \ingroup disjoint_sets_group
    template <class T>
    struct dso;
  
    /// \private
    /// \ingroup disjoint_sets_group
    /// T should support T += T
    template <class T>
    struct root {
      T data;
      mutable size_t rank;
  
      template <typename... Args>
      BAD(hd,inline)
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
  struct disjoint {
  private:
    using dso = detail::dso<T>;
    using root = detail::root<T>;
    mutable rc<dso> p;

  public:
    /// constructs a fresh disjoint set initialized with T's default constructor
    BAD(hd,inline)
    disjoint() noexcept;

    BAD(hd,inline)
    disjoint(disjoint const & rhs) noexcept
    : p(rhs.p) {}

    BAD(hd,inline)
    disjoint(disjoint && rhs) noexcept
    : p(std::move(rhs.p)) {}

  private:
    BAD(hd,inline)
    disjoint(dso * rhs) noexcept
    : p(rhs) {
      assert(rhs != nullptr);
    }

    friend dso;

  public:
    BAD(hd,inline)
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
    BAD(hd,inline)
    root & find() const noexcept;

  public:
    /// merge two disjoint sets
    BAD(hd,inline)
    disjoint & operator | (disjoint &) noexcept;

    /// the returned reference remains valid at least until a `merge` involves this disjoint set.
    BAD(hd,inline)
    T & value() noexcept {
      return find().data;
    }

    /// the returned reference remains valid at least until a `merge` involves this disjoint set.
    BAD(hd,inline)
    T const & value() const noexcept {
      return find().data;
    }

    /// do these two disjoint sets share the same root?
    BAD(hd,inline)
    friend bool operator == (
      BAD(noescape) disjoint const & lhs,
      BAD(noescape) disjoint const & rhs
    ) noexcept {
      return &lhs.find() == &rhs.find();
    }

    /// do these two disjoint sets not share the same root?
    BAD(hd,inline)
    friend bool operator != (
      BAD(noescape) disjoint const & lhs,
      BAD(noescape) disjoint const & rhs
    ) noexcept {
      return &lhs.find() != &rhs.find();
    }

  private:
    BAD(hd,inline)
    dso * operator ->() const noexcept {
      auto result = p.get();
      assert(result != nullptr);
      return result;
    }

    BAD(hd,inline)
    dso & operator *() const noexcept {
      auto result = p.get();
      assert(result != nullptr);
      return *result;
    }

    BAD(hd,inline)
    disjoint<T> parent() const noexcept;

    BAD(hd,inline)
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
    struct link {
      mutable disjoint<T> parent; // may be mutably forwarded by find, not user visible as such
  
      template <typename... Args>
      BAD(hd,inline)
      explicit link(Args...args) noexcept
      : parent(std::forward<Args>(args)...) {}
    };
  
    /// \private
    /// \ingroup disjoint_sets_group
    ///
    /// The selected policy for counted<dso<T>> means that these are _not_ thread safe.
    template <class T>
    struct dso : counted<dso<T>> {
      friend disjoint<T>;
      /// TODO: eventually carry a shared mutex to control access to entry
  
      using entry_type = std::variant<root<T>,link<T>>;
      entry_type entry;
  
      template <class... Args>
      BAD(hd,inline)
      explicit dso(Args&&... args) noexcept
      : counted<dso<T>>()
      , entry(std::forward<Args>(args)...) {}
  
      BAD(hd,inline,flatten)
      disjoint<T> parent() noexcept {
        dso * self = this;
        assert(self != nullptr);
        return std::visit([self](auto v) {
          if constexpr (std::is_same_v<decltype(v),link<T>>) {
            return v.parent;
          } else {
            return disjoint<T>(self);
          }
        },entry);
      }
  
      template <class ... Args>
      BAD(hd,inline,flatten)
      void set_parent(Args&& ... args) noexcept {
        entry.template emplace<link<T>>(std::forward<Args>(args)...);
      }
    };
  }

  template <class T>
  disjoint<T> disjoint<T>::parent() const noexcept {
    return disjoint(p->parent());
  }

  template <class T>
  void disjoint<T>::set_parent(disjoint<T> const & rhs) const noexcept {
    p->set_parent(rhs);
  }

  template <class T>
  disjoint<T>::disjoint() noexcept : p(new detail::dso<T>()) {}

  template <class T>
  disjoint<T>::disjoint(T const & t) noexcept
  : p (new detail::dso<T>(std::in_place_index_t<0>(),t)) {}

  template <class T>
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
  template <class T, class... Args>
  disjoint<T> & merge(disjoint<T> arg, Args...args) noexcept {
    return (arg | ... | args);
  }

  /// \ingroup disjoint_sets_group
  template <class T>
  disjoint<T> & merge() noexcept {
    return disjoint<T>();
  }
}

#endif
