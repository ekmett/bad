#ifndef BAD_STORAGE_STORE_HH
#define BAD_STORAGE_STORE_HH

#include "bad/sequences.hh"
#include "bad/storage/store_expr.hh"
#include "bad/storage/store_iterator.hh"

/// \file
/// \brief store implementation
/// \author Edward Kmett

namespace bad::storage {

  /// \ingroup storage_group
  template <class T, class Dim, class Stride = row_major<Dim>>
  struct store final {
    // offer slightly more helpful diagnostics first
    static_assert(std::is_same_v<typename Dim::value_type,size_t>, "expected dim to have type seq<...>");
    static_assert(std::is_same_v<typename Stride::value_type,ptrdiff_t>, "expected stride to have type sseq<...>");
    static_assert(seq_length<Dim> == seq_length<Stride>, "dim and stride have mismatched lengths");
    static_assert(no<T>, "only partial specializations are valid");
  };

  /// \ingroup storage_group
  template <class T>
  using scalar = store<T,seq<>,sseq<>>;

  /// scalars
  /// \ingroup storage_group
  template <class T>
  struct BAD(empty_bases,nodiscard) store<T, seq<>, sseq<>> final {
    using element = T;
    using dim = seq<>;
    using stride = sseq<>;
    static constexpr size_t rank = 0;

    template <size_t d0, ptrdiff_t s0>
    using ext = store<T,seq<d0>,sseq<s0>>;

    /// \meta
    template <auto j>
    struct tie_type {
      static constexpr ptrdiff_t step = 0;
      static constexpr size_t bias = 0;
      using type = store;
    };

    /// \meta
    template <auto j, size_t jd>
    using tied_type = tie_type<j>;

    T value;

    BAD(hd,inline)
    store()
    : value() {}

    BAD(hd,inline)
    store(T value)
    : value(value) {}

    BAD(hd,inline)
    store(const store & rhs)
    : value(rhs.value) {};

    BAD(hd,inline)
    store(store && rhs)
    : value(std::move(rhs.value)) {}

    BAD(hd,inline)
    store & operator =(const store & rhs) {
      value = rhs.value;
      return *this;
    }

    BAD(hd,inline)
    store & operator =(store && rhs) {
      value = std::move(rhs);
      return *this;
    }

    BAD(hd,nodiscard,inline) // const?
    T & at() noexcept {
      return value;
    }

    BAD(hd,nodiscard,inline,pure) // const?
    const T & at() const noexcept {
      return value;
    }

    template <typename arg, typename... args>
    BAD(hd,nodiscard,inline)
    const T & at(arg i, args... is) const noexcept {
      return value(i,is...);
    }

    template <typename arg, typename... args>
    BAD(hd,nodiscard,inline)
    auto at(arg i, args... is) noexcept {
      return value(i,is...);
    }

    BAD(hd,nodiscard,inline,pure) // const?
    T & operator()() noexcept {
      return value;
    }

    BAD(hd,nodiscard,inline,pure) // const?
    const T & operator()() const noexcept {
      return value;
    }

    template <typename arg, typename... args>
    BAD(hd,nodiscard,inline)
    T & operator()(arg i, args... is) noexcept {
      return value(i,is...);
    }

    template <typename arg, typename... args>
    BAD(hd,inline)
    const T & operator()(arg i, args... is) const noexcept {
      return value(i,is...);
    }

    BAD(hd,nodiscard,inline,pure) // const?
    operator T & () noexcept {
      return value;
    }

    BAD(hd,nodiscard,inline,pure) // const?
    operator T const & () const noexcept {
      return value;
    }
  };

  /// \ingroup storage_group
  template <typename T>
  BAD(hd,inline,flatten)
  void swap(
    BAD(noescape) scalar<T> & l,
    BAD(noescape) scalar<T> & r
  ) noexcept {
    using std::swap;
    swap(l.value,r.value);
  }

  /// tensors
  /// TODO: constructors and assignment should check for stride overlap (or at least 0 stride!)
  /// and static_assert if their are multiple indices that point to the same index.
  /// \ingroup storage_group
  template <class T, size_t d, size_t... ds, ptrdiff_t s, ptrdiff_t... ss>
  struct BAD(empty_bases,nodiscard) store<T, seq<d,ds...>, sseq<s,ss...>> final
  : store_expr<store<T, seq<d,ds...>, sseq<s,ss...>>,d,ds...> {

    using element = T;
    using dim = seq<d,ds...>;
    using stride = sseq<s,ss...>;
    using plane = store<T,seq<ds...>,sseq<ss...>>;
    using iterator = detail::store_iterator<d,s,T,plane>;
    using const_iterator = detail::store_iterator<d,s,T,plane>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    template <size_t d0, ptrdiff_t s0>
    using ext = store<T,seq<d0,d,ds...>,sseq<s0,s,ss...>>;

    static_assert(sizeof...(ss) == sizeof...(ds),"dim and stride do not have the same number of dimension");

    static constexpr size_t rank = 1 + sizeof...(ds);

    static constexpr size_t dim0 = d;
    static constexpr size_t stride0 = s;

    template <size_t i> static constexpr size_t nth_dim = nth<i,d,ds...>;
    template <size_t i> static constexpr ptrdiff_t nth_stride = nth<i,s,ss...>;
    template <size_t i> static constexpr ptrdiff_t nth_extremum = nth_stride<i>*(nth_dim<i>-1);

    template <class B>
    using expr = store_expr<B,d,ds...>;

    using super = expr<store>;

    /// \private
    template <class> struct calc_type;

    /// \private
    template <size_t... is>
    struct calc_type<seq<is...>> {
      static constexpr ptrdiff_t max = (0 + ... + std::max<ptrdiff_t>(0,nth_extremum<is>));
      static constexpr ptrdiff_t min = (0 + ... + std::min<ptrdiff_t>(0,nth_extremum<is>));
    };

    /// \private
    using calc = calc_type<make_seq<rank>>;
    /// offset in delta to apply when looking up the nth plane
    /// keep in mind planes can have higher strides than we do here!
    static constexpr size_t delta = std::max<ptrdiff_t>(0,s*(1-d));
    static constexpr size_t size = calc::max - calc::min + 1;

    T data[size]; ///< The ONLY data member allowed in this class

    BAD(hd,inline)
    constexpr store() noexcept
    : data() {}

    BAD(hd,inline)
    constexpr store(
      T value
    ) noexcept
    : data() {
      std::fill(begin(),end(),value);
    }

    BAD(hd,inline)
    constexpr store(
      std::initializer_list<T> list
    ) noexcept
    : data() {
      assert(list.size() <= d);
      std::copy(list.begin(),list.end(),begin());
    }

    template <class B>
    BAD(hd,inline,flatten)
    constexpr store(expr<B> const & rhs) noexcept {
      for (size_t i=0;i<d;++i)
        at(i) = rhs[i];
    }

    template <class A, class B, class... Cs>
    BAD(hd,inline)
    constexpr store(A a,B b,Cs...cs) noexcept : data() {
      static_assert(sizeof...(cs) + 2 <= d);
      auto i = begin();
      *i++ = a;
      *i++ = b;
      ((*i++ = cs),...,void());
    }

    // this should lifetimebound
    BAD(hd,nodiscard,inline,const)
    plane & operator[](size_t i) noexcept {
      return reinterpret_cast<plane &>(data[delta + i*s]);
    }

    BAD(hd,nodiscard,inline,const)
    plane const & operator[](size_t i) const noexcept {
      return reinterpret_cast<plane const &>(data[delta + i*s]);
    }

    BAD(hd,nodiscard,inline,const)
    store & at() noexcept {
      return *this;
    }

    BAD(hd,nodiscard,inline,const)
    store const & at() const noexcept {
      return *this;
    }

    template <class... ts>
    BAD(hd,nodiscard,inline,flatten) // this lifetimebound
    auto & at(size_t i) noexcept {
      return at()[i];
    }

    template <class... ts>
    BAD(hd,nodiscard,inline,flatten) // this lifetimebound
    auto & at(size_t i) const noexcept {
      return at()[i];
    }

    template <class... ts>
    BAD(hd,nodiscard,inline,flatten) // this lifetimebound
    auto at(size_t i, size_t j, ts... ks) noexcept {
      return at()[i](j, ks...);
    }

    template <class... ts>
    BAD(hd,nodiscard,inline,flatten) // this lifetimebound
    auto at(size_t i, size_t j, ts... ks) const noexcept {
      return at()[i](j, ks...);
    }

    BAD(hd,nodiscard,inline,const) // this lifetimebound
    store & operator()() noexcept {
      return at();
    }

    BAD(hd,nodiscard,inline,const) // this lifetimebound
    store const & operator()() const noexcept {
      return at();
    }

    template <class... ts>
    BAD(hd,nodiscard,inline,flatten) // this lifetimebound
    auto operator()(ts... is) noexcept {
      return at(is...);
    }


    template <class... ts>
    BAD(hd,nodiscard,inline,flatten) // this lifetimebound
    auto operator()(ts... is) const noexcept {
      return at(is...);
    }

    // this should match the behavior of the default = operator, which is to _0_ extend the initializer list
    // this happens because data is initialized in the initializer_list constructor
    // if the initializer_list too long, complain at runtime. otherwise we have a choice of semantics to 0 extend like arrays or
    BAD(hd,inline)
    store & operator = (
      std::initializer_list<T> list
    ) noexcept {
      assert(list.size() <= d);
      std::copy(list.begin(),list.end(),begin());
      for (size_t i = list.size();i<d; ++ i) {
        at(i) = 0;
      }
      return *this;
    }

    template <class B>
    BAD(reinitializes,hd,inline,flatten)
    store & operator = (expr<B> const & rhs) noexcept {
      for (size_t i=0;i<d;++i)
        at(i) = rhs[i];
      return *this;
    }

    BAD(hd,inline)
    store & operator += (
      std::initializer_list<T> list
    ) noexcept {
      assert(list.size() <= d);
      for (auto i=list.begin(),j=begin();i!=list.end();++i)
        *j += *i;
      return *this;
    }

    template <class B>
    BAD(hd,inline,flatten)
    store & operator += (expr<B> const & rhs) noexcept {
      for (size_t i=0;i<d;++i)
        at(i) += rhs[i];
      return *this;
    }

    BAD(hd,inline)
    store & operator -= (
      std::initializer_list<T> list
    ) noexcept {
      assert(list.size() <= d);
      for (auto i = list.begin(), j = begin();i != list.end();++i)
        *j -= *i;
      return *this;
    }

    template <class B>
    BAD(hd,inline,flatten)
    store & operator -= (expr<B> const & rhs) noexcept {
      for (size_t i=0;i<d;++i)
        at(i) -= rhs[i];
      return *this;
    }

    template <class B>
    BAD(hd,inline,flatten)
    store & operator *= (expr<B> const & rhs) noexcept {
      for (size_t i=0;i<d;++i)
        at(i) *= rhs[i];
      return *this;
    }

    template <size_t N>
    using store_pull = store<T, seq_pull<N,dim>, seq_pull<N,stride>>;

    template <size_t N>
    BAD(hd,nodiscard,inline,const)
    store_pull<N> & pull() noexcept {
      return reinterpret_cast<store_pull<N>&>(*this);
    };

    template <size_t N>
    BAD(hd,nodiscard,inline,const)
    const store_pull<N> & pull() const noexcept {
      return reinterpret_cast<store_pull<N> const &>(*this);
    };

    template <size_t N>
    BAD(hd,nodiscard,inline,flatten)
    typename store_pull<N>::plane & pull(size_t i) noexcept {
      return pull<N>()[i];
    };

    template <size_t N>
    BAD(hd,nodiscard,inline,flatten)
    typename store_pull<N>::plane const & pull(size_t i) const noexcept {
      return pull<N>()[i];
    }

    template <size_t N>
    BAD(hd,nodiscard,inline,const)
    auto & rep() const noexcept {
      using rep_type = store<T,seq_cons<N,dim>,seq_cons<ptrdiff_t(0),stride>>;
      return reinterpret_cast<rep_type const &>(*this);
    }

    template <size_t N>
    BAD(hd,nodiscard,inline,const)
    auto & rep() noexcept {
      using rep_type = store<T,seq_cons<N,dim>,seq_cons<ptrdiff_t(0),stride>>;
      return reinterpret_cast<rep_type &>(*this);
    }

    BAD(hd,nodiscard,inline,const) constexpr
    iterator begin() noexcept {
      return iterator(data + delta, 0);
    }

    BAD(hd,nodiscard,inline,const) constexpr
    iterator end() noexcept {
      return iterator(data + delta, d);
    }

    BAD(hd,nodiscard,inline,const) constexpr
    reverse_iterator rbegin() noexcept {
      return reverse_iterator(iterator(data + delta, d-1));
    }

    BAD(hd,nodiscard,inline,const) constexpr
    reverse_iterator rend() noexcept {
      return reverse_iterator(iterator(data + delta, -1));
    }

    BAD(hd,nodiscard,inline,const) constexpr
    const_iterator begin() const noexcept {
      return const_iterator(data + delta, 0);
    }

    BAD(hd,nodiscard,inline,const) constexpr
    const_iterator end() const noexcept {
      return const_iterator(data + delta, d);
    }

    BAD(hd,nodiscard,inline,const) constexpr
    const_iterator cbegin() const noexcept {
      return const_iterator(data + delta, 0);
    }

    BAD(hd,nodiscard,inline,const) constexpr
    const_iterator cend() const noexcept {
      return const_iterator(data + delta, d);
    }

    BAD(hd,nodiscard,inline,const) constexpr
    const_reverse_iterator rbegin() const noexcept {
      return reverse_iterator(const_iterator(data + delta, d-1));
    }

    BAD(hd,nodiscard,inline,const) constexpr
    const_reverse_iterator rend() const noexcept {
      return reverse_iterator(const_iterator(data + delta, -1));
    }

    BAD(hd,nodiscard,inline,const) constexpr
    const_reverse_iterator crbegin() const noexcept {
      return reverse_iterator(const_iterator(data + delta, d-1));
    }

    BAD(hd,nodiscard,inline,const) constexpr
    const_reverse_iterator crend() const noexcept {
      return reverse_iterator(const_iterator(data + delta, -1));
    }

    BAD(hd)
    friend std::ostream & operator<<(std::ostream &os, store const & rhs) {
      os << "{";
      for (size_t i=0;i<d;++i) {
        if (i) os << ",";
        os << rhs[i];
      }
      return os << "}";
    }

    /// \meta
    template <auto j, size_t jd, decltype(j)...is>
    struct tied_type ;

    /// \meta
    template <auto j, size_t jd>
    struct tied_type<j,jd> {
      static constexpr ptrdiff_t step = 0;
      static constexpr size_t bias = 0;
      using type = store;
    };

    /// \meta
    template <auto j, size_t jd, decltype(j) i, decltype(j)...is>
    struct tied_type<j,jd,i,is...> {
      static_assert((i != j) || (jd == d), "dimension mismatch");
      using p = typename plane::template tied_type<j,jd,is...>;
      static constexpr ptrdiff_t step = p::step + ((i==j)?s:0);
      static constexpr size_t bias = p::bias + ((i==j)?delta:0);
      using type = std::conditional_t<i == j, typename p::type, typename p::type::template ext<d,s>>;
    };

    template <auto j, size_t jd, decltype(j)...is>
    BAD(hd,inline,flatten)
    auto tied_begin() noexcept {
      // can relax this to <=, but using == for now
      static_assert(sizeof...(is) == sizeof...(ds), "fixing more dimensions than you have");
      using t = tied_type<j,jd,is...>;
      return detail::store_iterator<jd,t::step,T,typename t::type>(data + t::bias, 0);
    }

    template <auto j, size_t jd, decltype(j)...is>
    BAD(hd,inline,flatten)
    auto tied_begin() const noexcept {
      static_assert(sizeof...(is) == sizeof...(ds), "fixing more dimensions than you have");
      using t = tied_type<j,jd,is...>;
      return detail::const_store_iterator<jd,t::step,T,typename t::type>(data + t::bias, 0);
    }

    template <auto j, size_t jd, decltype(j)...is>
    BAD(hd,inline,flatten)
    auto tied_end() noexcept {
      static_assert(sizeof...(is) == sizeof...(ds), "fixing more dimensions than you have");
      using t = tied_type<j,jd,is...>;
      return detail::store_iterator<jd,t::step,T,typename t::type>(data + t::bias, jd);
    }


    template <auto j, size_t jd, decltype(j)...is>
    BAD(hd,inline,flatten)
    auto tied_end() const noexcept {
      static_assert(sizeof...(is) == sizeof...(ds), "fixing more dimensions than you have");
      using t = tied_type<j,jd,is...>;
      return detail::const_store_iterator<jd,t::step,T,typename t::type>(data + t::bias, jd);
    }

    template <auto j, size_t jd, decltype(j)...is>
    BAD(hd,inline,flatten)
    auto tied(size_t k) noexcept {
      return tied_begin<j,jd,is...>()[k];
    }

    template <auto j, size_t jd, decltype(j)...is>
    BAD(hd,inline,flatten)
    auto tied(size_t k) const noexcept {
      return tied_begin<j,jd,is...>()[k];
    }

    /// \meta
    template <auto j, decltype(j)...is>
    struct tie_type;

    /// \meta
    template <auto j>
    struct tie_type<j> {
      static constexpr ptrdiff_t step = 0;
      static constexpr size_t bias = 0;
      using type = store;
    };

    /// \meta
    template <auto j, decltype(j) i, decltype(j)...is>
    struct tie_type<j,i,is...> {
      using p = std::conditional_t<i==j, typename plane::template tied_type<j,d,is...>, typename plane::template tie_type<j,is...>>;
      static constexpr ptrdiff_t step = p::step + ((i==j)?s:0);
      static constexpr size_t bias = p::bias + ((i==j)?delta:0);
      using type = std::conditional_t<i==j, typename p::type, typename p::type::template ext<d,s>>;
    };

    template <auto j, decltype(j)...is>
    BAD(hd,inline,flatten)
    auto tie(size_t k) noexcept {
      static_assert(sizeof...(is) == 1 + sizeof...(ds), "fixing more dimensions than exist");
      using t = tie_type<j,is...>;
      return reinterpret_cast<typename t::type &>(data[t::bias + k * t::step]);
    }

    template <auto j, decltype(j)...is>
    BAD(hd,inline,flatten)
    auto tie(size_t k) const noexcept {
      static_assert(sizeof...(is) == 1 + sizeof...(ds), "fixing more dimensions than exist");
      using t = tie_type<j,is...>;
      return reinterpret_cast<typename t::type const &>(data[t::bias + k * t::step]);
    }
  }; // store

  /// scalar initialization
  /// \ingroup storage_group
  template <class T>
  store(const T &) -> store<T,seq<>,sseq<>>;

  // CTAD
 
  /// copy stride if copying from another store
  /// \ingroup storage_group
  template <class T, class dim, class stride>
  store(const store<T,dim,stride> &) -> store<T,dim,stride>;

  /// when fed one store_expression copy its dimensions, and pick a row_major order
  /// \ingroup storage_group
  template <class B, size_t d, size_t... ds>
  store(const store_expr<B,d,ds...> &) -> store<typename B::element,seq<d,ds...>>; // implicitly row_major

  /// when constructed from multiple storage expressions, use the length of the argument list to build the outermost dimension.
  /// \ingroup storage_group
  template <class B, size_t d, size_t...ds, class... U>
  store(const store_expr<B,d,ds...> &, U...) -> store<typename B::element,seq<1+sizeof...(U),d,ds...>>;

  /// vector construction
  /// \ingroup storage_group
  template <class T, class... U>
  store(T, U...) -> store<T,seq<1+sizeof...(U)>>;

  /// synonym, even though \ref store has better inference
  /// \ingroup storage_group
  template <class T, size_t N>
  using vec = store<T,seq<N>>;

  // for a more general version of the multiple storage expression rule, we'd need to be able to know dimensions for arbitrary types.
  // if we had something like:
  // dim<float> = seq<>, dim<store<T,Stride,Dim>> = Stride
  // dim<store_expr<B,d,ds...>> = seq<d,ds...>
  // base_type<B> to get at B::element as well
  // we could make this more robust
  // take shape and type from first arg,

  /// \ingroup storage_group
  template <typename T, typename stride1, typename stride2, size_t d, size_t... ds>
  BAD(hd,inline,flatten)
  void swap(
    BAD(noescape) store<T,seq<d,ds...>,stride1> & l,
    BAD(noescape) store<T,seq<d,ds...>,stride2> & r
  ) noexcept {
    using std::swap;
    for (size_t i=0;i<d;++i)
      swap(l[i],r[i]);
  }

  /// replicate base data types
  /// \ingroup storage_group
  template <size_t d, class T>
  BAD(hd,nodiscard,inline) constexpr
  auto rep(T t) noexcept -> store<T,seq<d>,sseq<0>> {
    return t;
  }
}

namespace bad {
  using namespace bad::storage;
}

#endif
