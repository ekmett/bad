#ifndef BAD_STORAGE_EINSUM_HH
#define BAD_STORAGE_EINSUM_HH

#include "store_expr.hh"

/// \file
/// \brief storage einsum impl

namespace bad::storage::detail {
  template <class BS, class CS>
  struct scalar_einsum;

  template <class I>
  struct scalar_einsum<iseq<I>, iseq<I>> {
    template <class B, class C>
    BAD(hd,inline,flatten)
    static auto apply(B b, C c) noexcept {
      return b*c;
    }
  };

  template <class I, I ci, I...cis>
  struct scalar_einsum<iseq<I>, iseq<I,ci,cis...>> {
    template <class B, class C>
    BAD(hd,inline,flatten)
    static auto apply(B b, C c) noexcept {
      using rest = scalar_einsum<iseq<I>,iseq<I,cis...>>;
      using T = decltype(rest::apply(b,c.template tie<ci,ci,cis...>(0)));
      T t = 0;
      for (size_t i=0;i<C::dim0;++i)
        t += rest::apply(b, c.template tie<ci,ci,cis...>(i));
      return t;
    }
  };

  template <class I, I bi, I... bis, I... cis>
  struct scalar_einsum<iseq<I,bi,bis...>, iseq<I,cis...>> {
    template <class B, class C>
    BAD(hd,inline,flatten)
    static auto apply(B b, C c) noexcept {
      using rest = scalar_einsum<iseq<I,bis...>,iseq<I,cis...>>;
      using T = decltype(rest::apply(b.template tie<bi,bi,bis...>(0),c.template tie<bi,cis...>(0)));
      T t = 0;
      for (size_t i=0;i<B::dim0;++i)
        t += rest::apply(b.template tie<bi,bi,bis...>(i),c.template tied<bi,B::dim0,cis...>(i));
      return t;
    }
  };

  template <class AS, class BS, class CS, class B, class C>
  using einsum_dim = seq<>; // TODO: look up each index in AS inside BS and CS and then find the corresponding entries in B::dim, C::dim

  // calculate AD from AS, BS, CS, B, and C by default, only needs to be explicitly stated when indices in AS don't occur in B and C
  // TODO: track a mask of boring dimensions such as indices in AS that don't occur in BS and CS so that we could use stride 0 in the backing store?
  // this would let us generate results more efficiently, at the risk of clobbering operations?
  // on top of that we should be able to use einstein summation notation conventions to fill in AS as well:
  // any index mentioned 2+ times is being removed. any index mentioned 1 time is not.
  // https://www.ita.uni-heidelberg.de/~dullemond/lectures/tensor/tensor.pdf
  template <class AS, class BS, class CS,class B, class C, class AD = einsum_dim<AS,BS,CS,B,C>>
  struct store_einsum_expr;

  template <class I, I ai, I... ais, I... bis, I... cis, class B, class C, size_t ad, size_t... ads>
  struct BAD(empty_bases) store_einsum_expr<iseq<I,ai,ais...>,iseq<I,bis...>,iseq<I,cis...>,B,C,seq<ad,ads...>>
  : store_expr<           store_einsum_expr<iseq<I,ai,ais...>,iseq<I,bis...>,iseq<I,cis...>,B,C,seq<ad,ads...>>,ad,ads...> {
    static_assert(sizeof...(ais) == sizeof...(ads), "store_einsum_expr: bad result rank");
    static_assert(sizeof...(ais) == seq_length<filter_ne<ai,ais...>>, "store_einsum_expr: duplicate result index"); // TODO: kroenecker test these instead?

    using b_type = std::decay_t<B>;
    using c_type = std::decay_t<C>;
    sub_expr<B> b;
    sub_expr<C> c;

    BAD(hd,inline,flatten)
    auto operator[](size_t i) const noexcept {
      auto bi = b.template tied<ai,ad,bis...>(i);
      auto ci = c.template tied<ai,ad,cis...>(i);
      return store_einsum_expr<
        iseq<I,ais...>,
        filter_ne<ai,bis...>,
        filter_ne<ai,cis...>,
        std::add_rvalue_reference_t<decltype(bi)>,
        std::add_rvalue_reference_t<decltype(ci)>,
        seq<ads...>
      >(bi,ci);
    }
    // TODO: implement the rest of the store_expr api
  };

  template <class I, I... bis, I...cis, class B, class C>
  struct BAD(empty_bases) store_einsum_expr<iseq<I>,iseq<I,bis...>,iseq<I,cis...>,B,C,seq<>> {
    using scalar_einsum_type = scalar_einsum<iseq<I,bis...>,iseq<I,cis...>>;
    using result_type = decltype(scalar_einsum_type::apply(std::declval<B const &>(),std::declval<C const &>()));

    using b_type = std::decay_t<B>;
    using c_type = std::decay_t<C>;

    const result_type value;

    BAD(hd,inline,flatten)
    store_einsum_expr(b_type const & b, c_type const & c)
    : value(scalar_einsum_type::apply(b,c)) {}

    BAD(hd,inline)
    operator const result_type () const {
      return value;
    }
  };
}

namespace bad::storage {

  template <class AS, class BS, class CS, class AD = seq<>, class B, class C>
  BAD(hd,inline)
  auto einsum(
    BAD(lifetimebound) B const & l,
    BAD(lifetimebound) C const & r
  ) noexcept -> detail::store_einsum_expr<AS,BS,CS,B,C,AD> {
    return { {}, l.at(), r.at() };
  }

  template <class AS, class BS, class CS, class AD = seq<>, class B, class C>
  BAD(hd,inline)
  auto einsum(
    BAD(lifetimebound) B const & l,
    BAD(noescape) C && r
  ) noexcept -> detail::store_einsum_expr<AS,BS,CS,B,C&&,AD> {
    return { {}, l.at(), r.at() };
  }

  template <class AS, class BS, class CS, class AD = seq<>, class B, class C>
  BAD(hd,inline)
  auto einsum(
    BAD(noescape) B && l,
    BAD(lifetimebound) C const & r
  ) noexcept -> detail::store_einsum_expr<AS,BS,CS,B&&,C,AD> {
    return { {}, l.at(), r.at() };
  }

  template <class AS, class BS, class CS, class AD = seq<>, class B, class C>
  BAD(hd,inline)
  auto einsum(
    BAD(noescape) B && l,
    BAD(noescape) C && r
  ) noexcept -> detail::store_einsum_expr<AS,BS,CS,B&&,C&&,AD> {
    return { {}, l.at(), r.at() };
  }
}

namespace bad {
  using namespace bad::storage;
}

#endif
