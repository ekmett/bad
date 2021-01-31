#pragma once
#include "storage.hh"

namespace bad {
  // TODO: convert this to a store_expr
  // then we can drop the first arg, and stride becomes irrelevant
  // in addition unused parts won't get built

  // bs...,cs...->as... accumulating with += into the target
  template <class AS, class BS, class CS>
  struct BAD(empty_bases) store_einsum {
    static_assert(no<AS>,"store_einsum: failed specialization");
  };

  template <class I>
  struct BAD(empty_bases) store_einsum<iseq<I>, iseq<I>, iseq<I>> {
    template <class AT, class BT, class CT>
    BAD(hd,inline,flatten)
    void operator()(AT & a, BT b, CT c) noexcept {
      a += b * c;
    }
  };

  template <class I, I ci, I...cis>
  struct BAD(empty_bases) store_einsum<iseq<I>, iseq<I>, iseq<I,ci,cis...>> {
    template <class AT, class BT, class C, size_t cd, size_t... cds>
    BAD(hd,inline,flatten)
    void operator()(
      BAD(noescape) AT & a, 
      BT b,
      BAD(noescape) store_expr<C,cd,cds...> const & c
    ) noexcept {
      static_assert(sizeof...(cis) == sizeof...(cds), "store_einsum: right arity mismatch");
      for (size_t i = 0;i<cd;++i) {
        store_einsum<iseq<I>,iseq<I>,filter_ne<ci,cis...>> op;
        op(a,b,c.template tied<ci,cd,ci,cis...>(i));
      }
    }
  };

  template <class I, I bi, I...bis, I...cis>
  struct BAD(empty_bases) store_einsum<iseq<I>, iseq<I,bi,bis...>, iseq<I,cis...>> {
    template <class AT, class B, size_t bd, size_t... bds, class C, size_t... cds>
    BAD(hd,inline,flatten)
    void operator()(
      BAD(noescape) AT & a, 
      BAD(noescape) store_expr<B,bd,bds...> const & b,
      BAD(noescape) store_expr<C,cd,cds...> const & c
    ) noexcept {
      static_assert(sizeof...(bis) == sizeof...(bds), "store_einsum: left arity mismatch");
      static_assert(sizeof...(cis) == sizeof...(cds), "store_einsum: right arity mismatch");
      for (size_t i = 0;i<bd;++i) {
        store_einsum<iseq<I>,filter_ne<bi,bis...>, filter_ne<bi,cis...>> op;
        op(a, b.template tied<bi,bd,bi,bis...>(i), c.template tied<bi,bd,cis...>(i));
      }
    }
  };

  template <class I, I ai, I...ais, I...bis, I...cis>
  struct BAD(empty_bases) store_einsum<iseq<I,ai,ais...>,iseq<I,bis...>,iseq<I,cis...>> {
    template <class T, size_t ad, size_t... ads, class Stride, class B, size_t... bds, class C, size_t... cds>
    BAD(hd,inline,flatten)
    void operator()(
      BAD(noescape) store<T,seq<ad,ads...>,Stride> & a,
      BAD(noescape) store_expr<B,bds...> const & b,
      BAD(noescape) store_expr<C,cds...> const & c 
    ) noexcept {
      static_assert(sizeof...(ais) == sizeof...(ads), "store_einsum: result arity mismatch");
      static_assert(sizeof...(bis) == sizeof...(bds), "store_einsum: left arity mismatch");
      static_assert(sizeof...(cis) == sizeof...(cds), "store_einsum: right arity mismatch");
      static_assert(sizeof...(ais) == sizeof...(filter_ne<ai,ais>), "store_einsum: duplicate result index");
      for (size_t i = 0;i<ad;++i) {
        store_einsum<iseq<I,ais...>,filter_ne<ai,bis...>, filter_ne<ai,cis...>> op;
        op(a[i],b.template tied<ai,ad,bis...>(i),c.template tied<ai,ad,cis...>(i));
      }
    }
  };
}
