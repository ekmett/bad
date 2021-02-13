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
  struct unit {
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
      rc<lco> path, parent, left, right;
      T value;
      T summary;

      BAD(hd,inline)
      lco() noexcept
      : path(), parent(), left(), right(), summary() {}

      BAD(hd,inline)
      lco(T v) noexcept
      : path(), parent(), left(), right(), value(v), summary(v) {}

      BAD(hd,inline)
      void cut() noexcept {
        if (left) {
          auto l = left;
          left = nullptr;
          l->parent = nullptr;
          summary = value;
        }
      }

      BAD(hd,inline)
      void link(
        rc<lco> const & rhs
      ) noexcept {
        assert(rhs);
        access();
        rhs->access();
        path = rhs;
      }

      BAD(hd,inline)
      bool connected(
        rc<lco> const & rhs
      ) noexcept {
        return rhs && root() == rhs->root();
      }

      // SFINAE 
      BAD(hd,inline)
      T cost() noexcept {
        access();
        return summary;
      }

      BAD(hd,inline)
      rc<lco> root() noexcept {
        access();
        lco * v = this; // using existing rcs
        {
          lco * l = v->left.get();
          while (l) {
            v = l;
            l = v->left.get();
          }
        } // v is now leftmost
        v->splay();
        return v; // bumps rc
      }

      BAD(hd,inline)
      rc<lco> up() noexcept {
        access();
        if (!left) return nullptr;
        lco * v = this; // not null
        {
          lco * r = v->right.get();
          while (r) {
            v = r;
            r = v->right.get();
          }
        } // v is now rightmost
        v->splay();
        return v; // bumps rc
      }

      BAD(hd) // not inline! holy cow
      void access() noexcept {
        splay();
        if (right) {
          auto r = right;
          right = nullptr;
          r->parent = nullptr;
          r->path = this;
          summary = left ? left->summary + value : value;
        }
        lco * v = this; // not null
        {
          auto w = v->path;
          while (w) {
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
            v = w.get();
            w = v->path;
          }
        }
        splay();
      }

      BAD(hd,inline)
      static T summarize(rc<lco> const & p) {
        return p ? p->summary : T();
      }

      BAD(hd)
      void splay() noexcept {
        for (;;) {
          auto p = parent;
          if (p) {
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
    void link(link_cut & rhs) noexcept {
      assert(p && rhs.p);
      p->link(rhs.p);
    }

    BAD(hd,inline)
    bool connected(link_cut & rhs) noexcept {
      return p && rhs.p && p->connected(rhs.p);
    }

    BAD(hd,inline)
    T summary() noexcept {
      return lco::summarize(p);
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
}

namespace bad {
  using namespace bad::links;
}

#endif
