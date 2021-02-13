#ifndef BAD_TREES_LINK_CUT_HH
#define BAD_TREES_LINK_CUT_HH

#include "bad/memory.hh"

/// \file
/// link cut trees
///
/// \defgroup link_cut_group link-cut trees
/// \ingroup trees_group 
/// link cut trees

namespace bad::trees {
  /// \ingroup link_cut_groups
  struct BAD(empty_bases) unit final {
    BAD(hd,inline)
    friend unit operator + (
      BAD(maybe_unused) unit,
      BAD(maybe_unused) unit
    ) noexcept {
      return {};
    }
  };

  // specializations should have an operator() that takes a T and returns some other type
  // use this like std::hash<T>{}(x)
  template <class T>
  struct measure {
    BAD(hd,inline)
    T operator()(T x) { return x; }
  };

  namespace detail {
    /// \ingroup link_cut_groups
    /// \meta
    /// requires `U = measure(T)`, `U()`, `U = U + U` for some type U
   
    // TODO: parameterize on policy?
    template <class T = unit>
    struct lco final : intrusive_target<lco<T>> {
    private:
      lco * path;
      lco * parent;
      lco * left;
      lco * right;
      T value;

    public:
      using measure_type = decltype(::bad::trees::measure<T>{}(std::declval<T>()));

    private:
      measure_type summary;

      BAD(hd,inline)
      static measure_type measure(T x) {
        return ::bad::trees::measure<T>{}(x);
      }


    public:
      BAD(hd,inline)
      lco() noexcept
      : path(), parent(), left(), right(), value(), summary(measure(value)) {}

      BAD(hd,inline)
      lco(T v) noexcept
      : path(), parent(), left(), right(), value(v), summary(measure(value)) {}

      BAD(hd,inline)
      void cut() noexcept {
        access();
        if (left) {
          auto l = left;
          left = nullptr;
          l->parent = nullptr;
          summary = measure(value);
          release(l); // reduce the reference count of the thing formerly above by one
        }
      }

      BAD(hd,inline)
      void link(
        lco * rhs
      ) noexcept {
        assert(rhs);

        access();
        rhs->access();
        path = rhs;
        acquire(rhs); // increase the reference count of our new parent by one
      }

      BAD(hd,inline)
      bool connected(
        lco * rhs
      ) noexcept {
        return rhs && root() == rhs->root();
      }

      // SFINAE
      BAD(hd,inline)
      measure_type cost() noexcept {
        access();
        // access placed us at the top, summary tallies everything below us by definition
        return summary;
      }

      BAD(hd,inline)
      lco * root() noexcept {
        access();
        lco * v = this; // using existing rcs
        lco * l = v->left;
        while (l) {
          v = l;
          l = v->left;
        }
        v->splay();
        return v; // bumps rc
      }

      BAD(hd,inline)
      lco * up() noexcept {
        access();
        if (!left) return nullptr;
        lco * v = this; // not null
        lco * r = v->right;
        while (r) {
          v = r;
          r = v->right;
        }
        // v is now rightmost
        v->splay();
        return v; // bumps rc
      }

      BAD(hd) // not inline! holy cow
      lco * access() noexcept {
        splay();
        if (right) {
          auto r = right;
          right = nullptr;
          r->parent = nullptr;
          r->path = this;
          summary = left ? left->summary + measure(value) : measure(value);
        }
        lco * v = this; // not null
        lco * last_w = v;
        auto w = v->path;
        while (w) {
          last_w = w;
          w->splay();
          auto b = w->right;
          if (b) {
            b->path = w;
            b->parent = nullptr;
          }
          auto a = w->left;
          w->summary = a ? a->summary + measure(w->value) + v->summary : measure(w->value) + v->summary;
          v->parent = w;
          w->right = v;
          v = w;
          w = v->path;
        }
        splay();
        return last_w;
      }

      BAD(hd,inline)
      lco * lca(lco * rhs) noexcept {
        assert(rhs);
        // assert(connected(rhs));

        access();
        return rhs->access();
      }

      BAD(hd,inline)
      static measure_type summarize(lco * p) {
        return p ? p->summary : measure_type();
      }

      BAD(hd)
      void splay() noexcept {
        for (;;) {
          auto p = parent;
          if (!p) return;
          if (!p->parent) { // zig
            p->parent = this;
            parent = nullptr;
            path = p->path;
            p->path = nullptr;
            summary = p->summary;
            if (p->left == this) {
               //    p           x
               //  x   d  ==>  b   p
               // b c             c d
               auto c = right;
               if (c) c->parent = p;
               right = p;
               p->left = c;
               p->summary = summarize(c) + measure(p->value) + summarize(p->right);
            } else {
               //   p            x
               // a   x   ==>  p   c
               //    b c      a b
               auto b = left;
               if (b) b->parent = p;
               left = p;
               p->right = b;
               p->summary = summarize(p->left) + measure(p->value) + summarize(b);
            }
            return; // can't go higher
          } else { // g is not nullptr, zig-zig or zig-zag
            auto g = p->parent;
            auto gg = g->parent;
            summary = g->summary;
            parent = gg;
            path = g->path;
            g->path = nullptr;
            if (g->left == p) {
              if (p->left == this) { // zig-zig
                // --      g       x
                // --    p  d     a  p
                // --  x  c   ==>   b  g
                // -- a b             c d
                auto b = right;
                auto c = p->right;
                p->parent = this;
                g->parent = p;
                if (b) b->parent = p;
                if (c) c->parent = g;
                right = p;
                p->right = g;
                p->left = b;
                g->left = c;
                auto sg = summarize(c) + measure(g->value) + summarize(g->right);
                g->summary = sg;
                p->summary = summarize(b) + measure(p->value) + sg;
              } else { // zig-zag
                // --       g           x
                // --   p    d  ==>   p   g
                // --  a  x          a b c d
                // --    b c
                auto b = left;
                auto c = right;
                p->parent = this;
                g->parent = this;
                if (b) b->parent = p;
                if (c) c->parent = g;
                left = p;
                right = g;
                p->right = b;
                g->left = c;
                p->summary = summarize(p->left) + measure(p->value) + summarize(b);
                g->summary = summarize(c) + measure(g->value) + summarize(g->right);
              }
            } else if (p->left == this) { // gl != p, zag-zig
              // --   g               x
              // --  a    p         g   p
              // --     x  d  ==>  a b c d // d = p->right
              // --    b c
              auto b = left;
              auto c = right;
              g->parent = this;
              p->parent = this;
              if (b) b->parent = g;
              if (c) c->parent = p;
              left = g;
              right = p;
              g->right = b;
              p->left = c;
              g->summary = summarize(g->left) + measure(g->value) + summarize(b);
              p->summary = summarize(c) + measure(p->value) + summarize(p->right);
            } else { // gl != p, pl != x, zag-zag
              // --  g               x
              // -- a  p           p  d
              // --   b  x  ==>  g  c
              // --     c d     a b
              auto b = p->left;
              auto c = left;
              if (b) b->parent = g;
              if (c) c->parent = p;
              p->parent = this;
              g->parent = p;
              left = p;
              p->left = g;
              g->right = b;
              p->right = c;
              auto sg = summarize(g->left) + measure(g->value) + summarize(b);
              g->summary = sg;
              p->summary = sg + measure(p->value) + summarize(c);
            }
            if (!gg) return;

            if (gg->left == g) gg->left = this;
            else gg->right = this;
          }
        } // for(;;)
      } // splay()
    }; // struct lco
  } // namespace detail

  /// \ingroup link_cut_group
  /// link-cut trees
  template <class T>
  struct link_cut final {
  private:
    using lco = detail::lco<T>;
    intrusive_ptr<lco> p;

    BAD(hd,inline)
    link_cut(lco * p) noexcept
    : p(p) {}

  public:

    using measure_type = typename detail::lco<T>::measure_type;

    BAD(hd,inline)
    link_cut() noexcept
    : p(new lco()) {}

    BAD(hd,inline)
    link_cut(T v) noexcept
    : p(new lco(v)) {}

    BAD(hd,inline)
    link_cut(
      BAD(noescape) link_cut const & rhs
    ) noexcept : p(rhs.p) {}

    BAD(hd,inline)
    link_cut(
      BAD(noescape) link_cut && rhs
    ) noexcept : p(std::move(rhs.p)) {}

    BAD(hd,inline)
    link_cut & operator = (
      BAD(noescape) link_cut const & rhs
    ) noexcept {
      p = rhs.p;
      return *this;
    }

    BAD(hd,inline)
    link_cut & operator = (
      BAD(noescape) link_cut && rhs
    ) noexcept {
      p = std::move(rhs.p);
      return *this;
    }

    BAD(hd,inline)
    void cut() noexcept {
      assert(p);
      p->cut();
    }

    BAD(hd,inline)
    void link(link_cut rhs) noexcept {
      assert(p && rhs.p);
      p->link(rhs.p.get());
    }

    BAD(hd,inline)
    bool connected(link_cut rhs) noexcept {
      return p && rhs.p && p->connected(rhs.p.get());
    }

    BAD(hd,inline)
    measure_type cost() noexcept {
      return p ? p->cost() : measure_type();
    }

    BAD(hd,inline)
    link_cut lca(link_cut rhs) noexcept {
      return { p->lca(rhs.p.get()) };
    }

    BAD(hd,inline)
    T value() noexcept {
      assert(p);
      return p->value();
    }
    // TODO: lca

    BAD(hd,inline)
    friend bool operator == (
      BAD(noescape) link_cut const & lhs,
      BAD(noescape) link_cut const & rhs
    ) noexcept {
      return lhs.p == rhs.p;
    }

    BAD(hd,inline)
    friend bool operator != (
      BAD(noescape) link_cut const & lhs,
      BAD(noescape) link_cut const & rhs
    ) noexcept {
      return lhs.p != rhs.p;
    }
  };

  template <class T>
  BAD(hd,inline,flatten)
  void swap(
    BAD(noescape) link_cut<T> & lhs,
    BAD(noescape) link_cut<T> & rhs
  ) noexcept {
    using std::swap;
    swap(lhs.p,rhs.p);
  }

  // TODO: printf support

  /// \ingroup link_cut_group
  template <class T>
  BAD(hd,inline)
  void cut(link_cut<T> & p) noexcept {
    p.cut();
  }

  /// \ingroup link_cut_group
  template <class T>
  BAD(hd,inline)
  link_cut<T> link(link_cut<T> & p, link_cut<T> & q) noexcept {
    return p.link(q);
  }

  /// \ingroup link_cut_group
  template <class T>
  BAD(hd,inline)
  link_cut<T> root(link_cut<T> & p) noexcept {
    return p.root();
  }

  /// \ingroup link_cut_group
  template <class T>
  BAD(hd,inline)
  auto cost(link_cut<T> & p) noexcept -> typename link_cut<T>::measure_type {
    return p.cost();
  }
}

namespace bad {
  using namespace bad::trees;
}

#endif
