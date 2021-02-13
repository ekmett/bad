#ifndef BAD_LINKS_HH
#define BAD_LINKS_HH

#include "bad/memory.hh"

/// \file
/// link cut trees
///
/// \defgroup links_group link-cut trees
/// link cut trees

namespace bad::links {
  /// \ingroup links_groups
  struct BAD(empty_bases) unit {
    BAD(hd,inline)
    friend unit operator + (
      BAD(maybe_unused) unit,
      BAD(maybe_unused) unit
    ) noexcept {
      return {};
    }
  };

  namespace detail {
    /// \ingroup links_groups
    /// \meta
    template <class T = unit>
    struct lco : counted<lco<T>> {
      lco * path;
      lco * parent;
      lco * left;
      lco * right;
      T value;
      T summary;

      BAD(hd,inline)
      lco() noexcept
      : path(), parent(), left(), right(), value(), summary() {}

      BAD(hd,inline)
      lco(T v) noexcept
      : path(), parent(), left(), right(), value(v), summary(v) {}

      BAD(hd,inline)
      void cut() noexcept {
        access();
        if (left) {
          auto l = left;
          left = nullptr;
          l->parent = nullptr;
          summary = value;
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
      T cost() noexcept {
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
          summary = left ? left->summary + value : value;
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
          w->summary = a ? a->summary + w->value + v->summary : w->value + v->summary;
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
      static T summarize(lco * p) {
        return p ? p->summary : T();
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
               p->summary = summarize(c) + p->value + summarize(p->right);
            } else {
               //   p            x
               // a   x   ==>  p   c
               //    b c      a b
               auto b = left;
               if (b) b->parent = p;
               left = p;
               p->right = b;
               p->summary = summarize(p->left) + p->value + summarize(b);
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
                auto sg = summarize(c) + g->value + summarize(g->right);
                g->summary = sg;
                p->summary = summarize(b) + p->value + sg;
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
                p->summary = summarize(p->left) + p->value + summarize(b);
                g->summary = summarize(c) + g->value + summarize(g->right);
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
              g->summary = summarize(g->left) + g->value + summarize(b);
              p->summary = summarize(c) + p->value + summarize(p->right);
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
              auto sg = summarize(g->left) + g->value + summarize(b);
              g->summary = sg;
              p->summary = sg + p->value + summarize(c);
            }
            if (!gg) return;

            if (gg->left == g) gg->left = this;
            else gg->right = this;
          }
        } // for(;;)
      } // splay()
    }; // struct lco
  } // namespace detail

  /// \ingroup links_group
  /// link-cut trees
  template <class T>
  struct link_cut {
  private:
    using lco = detail::lco<T>;
    rc<lco> p;

    BAD(hd,inline)
    link_cut(lco * p) noexcept
    : p(p) {}

  public:
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
    T cost() noexcept {
      return p ? p->cost() : T();
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

  /// \ingroup links_group
  template <class T>
  BAD(hd,inline)
  void cut(link_cut<T> & p) noexcept {
    p.cut();
  }

  /// \ingroup links_group
  template <class T>
  BAD(hd,inline)
  auto link(link_cut<T> & p, link_cut<T> & q) noexcept {
    return p.link(q);
  }

  /// \ingroup links_group
  template <class T>
  BAD(hd,inline)
  auto root(link_cut<T> & p) noexcept {
    return p.root();
  }

  /// \ingroup links_group
  template <class T>
  BAD(hd,inline)
  auto cost(link_cut<T> & p) noexcept {
    return p.cost();
  }
}

namespace bad {
  using namespace bad::links;
}

#endif
