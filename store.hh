#pragma once
#include "store_expr.hh"

#pragma STDC FP_CONTRACT ON

namespace bad {

  template <class T, class Dim, class Stride = row_major<Dim>>
  struct store {
    // offer slightly more helpful diagnostics first
    static_assert(std::is_same_v<seq_element_type<Dim>,size_t>, "expected dim to have type seq<...>");
    static_assert(std::is_same_v<seq_element_type<Stride>,ptrdiff_t>, "expected stride to have type sseq<...>");
    static_assert(seq_length<Dim> == seq_length<Stride>, "dim and stride have mismatched lengths");
    static_assert(no<T>, "only partial specializations are valid");
  };

  template <class T>
  using scalar = store<T,seq<>,sseq<>>;

  // scalar<T> is just a proxy for T
  template <class T>
  struct store<T, seq<>, sseq<>> {
    using element = T;
    using dim = seq<>;
    using stride = sseq<>;
    static constexpr size_t arity = 0;

    BAD(hd,inline)
    store() : value() {}

    BAD(hd,inline)
    store(T value) : value(value) {}

    BAD(hd,inline)
    store(const store & rhs) : value(rhs.value) {};

    BAD(hd,inline)
    store(store && rhs) : value(std::move(rhs.value)) {}

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

    T value;

    BAD(hd,inline) // const?
    T & at() noexcept { return value; }

    BAD(hd,inline,pure) // const?
    const T & at() const noexcept { return value; }

    template <typename arg, typename... args>
    BAD(hd,inline)
    const T & at(arg i, args... is) const noexcept { return value(i,is...); }

    template <typename arg, typename... args>
    BAD(hd,inline)
    auto at(arg i, args... is) noexcept { return value(i,is...); }


    BAD(hd,inline,pure) // const?
    T & operator()() noexcept { return value; }

    BAD(hd,inline,pure) // const?
    const T & operator()() const noexcept { return value; }

    template <typename arg, typename... args>
    BAD(hd,inline)
    T & operator()(arg i, args... is) noexcept { return value(i,is...); }

    template <typename arg, typename... args>
    BAD(hd,inline)
    const T & operator()(arg i, args... is) const noexcept { return value(i,is...); }



    BAD(hd,inline,pure) // const?
    operator T & () noexcept { return value; }

    BAD(hd,inline,pure) // const?
    operator T const & () const noexcept { return value; }
  };

  template <typename T>
  BAD(hd,inline,flatten)
  void swap(
    BAD(noescape) scalar<T> & l,
    BAD(noescape) scalar<T> & r
  ) noexcept {
    using std::swap;
    swap(l.value,r.value);
  }

  // tensor case
  template <class T, size_t d, size_t... ds, ptrdiff_t s, ptrdiff_t... ss>
  struct store<T, seq<d,ds...>, sseq<s,ss...>>
       : store_expr<store<T, seq<d,ds...>, sseq<s,ss...>>,d,ds...> {
    using element = T;
    using dim = seq<d,ds...>;
    using stride = sseq<s,ss...>;
    using plane = store<T,seq<ds...>,sseq<ss...>>;

    static_assert(sizeof...(ss) == sizeof...(ds),"dim and stride do not have the same number of dimension");

    static constexpr size_t arity = 1 + sizeof...(ds);

    template <size_t i> static constexpr size_t nth_dim = nth<i,d,ds...>;
    template <size_t i> static constexpr ptrdiff_t nth_stride = nth<i,s,ss...>;
    template <size_t i> static constexpr ptrdiff_t nth_extremum = nth_stride<i>*(nth_dim<i>-1);

    template <class B> using expr = store_expr<B,d,ds...>;

    using super = expr<store>;

    template <class> struct calc_t;

    template <size_t... is>
    struct calc_t<seq<is...>> {
      static constexpr ptrdiff_t max = (0 + ... + std::max<ptrdiff_t>(0,nth_extremum<is>));
      static constexpr ptrdiff_t min = (0 + ... + std::min<ptrdiff_t>(0,nth_extremum<is>));
    };

    using calc = calc_t<make_seq<arity>>;
    // offset in delta to apply when looking up the nth plane
    // keep in mind planes can have higher strides than we do here!
    static constexpr size_t delta = std::max<ptrdiff_t>(0,s*(1-d));
    //static constexpr size_t offset = - calc::min;
  public:
    static constexpr size_t size = calc::max - calc::min + 1;

    T data[size]; // this is the ONLY data member allowed in this class

    BAD(hd,inline)
    constexpr store() noexcept : data() {
    }

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
    BAD(hd,inline)
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
    BAD(hd,inline,const)
    plane & operator[](size_t i) noexcept {
      return reinterpret_cast<plane &>(data[delta + i*s]);
    }

    BAD(hd,inline,const)
    plane const & operator[](size_t i) const noexcept {
      return reinterpret_cast<plane const &>(data[delta + i*s]);
    }

    BAD(hd,inline,const)
    store & at() noexcept {
      return *this;
    }

    BAD(hd,inline,const)
    store const & at() const noexcept {
      return *this;
    }

    template <class... ts>
    BAD(hd,inline,flatten) // this lifetimebound
    auto & at(size_t i) noexcept {
      return at()[i];
    }

    template <class... ts>
    BAD(hd,inline,flatten) // this lifetimebound
    auto & at(size_t i) const noexcept {
      return at()[i];
    }

    template <class... ts>
    BAD(hd,inline,flatten) // this lifetimebound
    auto at(size_t i, size_t j, ts... ks) noexcept {
      return at()[i](j, ks...);
    }

    template <class... ts>
    BAD(hd,inline,flatten) // this lifetimebound
    auto at(size_t i, size_t j, ts... ks) const noexcept {
      return at()[i](j, ks...);
    }

    BAD(hd,inline,const) // this lifetimebound
    store & operator()() noexcept {
      return at();
    }

    BAD(hd,inline,const) // this lifetimebound
    store const & operator()() const noexcept {
      return at();
    }

    template <class... ts>
    BAD(hd,inline,flatten) // this lifetimebound
    auto operator()(ts... is) noexcept {
      return at(is...);
    }


    template <class... ts>
    BAD(hd,inline,flatten) // this lifetimebound
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
    BAD(hd,inline,const)
    store_pull<N> & pull() noexcept {
      return reinterpret_cast<store_pull<N>&>(*this);
    };

    template <size_t N>
    BAD(hd,inline,const)
    const store_pull<N> & pull() const noexcept {
      return reinterpret_cast<store_pull<N> const &>(*this);
    };

    template <size_t N>
    BAD(hd,inline,flatten)
    typename store_pull<N>::plane & pull(size_t i) noexcept {
      return pull<N>()[i];
    };

    template <size_t N>
    BAD(hd,inline,flatten)
    typename store_pull<N>::plane const & pull(size_t i) const noexcept {
      return pull<N>()[i];
    }

    template <size_t N>
    BAD(hd,inline,const)
    auto & rep() const noexcept {
      using rep_t = store<T,seq_cons<N,dim>,seq_cons<ptrdiff_t(0),stride>>;
      return reinterpret_cast<rep_t const &>(*this);
    }

    template <size_t N>
    BAD(hd,inline,const)
    auto & rep() noexcept {
      using rep_t = store<T,seq_cons<N,dim>,seq_cons<ptrdiff_t(0),stride>>;
      return reinterpret_cast<rep_t &>(*this);
    }

    struct const_iterator {
      using value_type = plane const;
      using difference_type = ptrdiff_t;
      using pointer = value_type *;
      using reference = value_type &;
      using iterator_category = std::random_access_iterator_tag;

      BAD(hd,inline,noalias)
      constexpr explicit const_iterator(T const * p = nullptr, difference_type i = 0) noexcept
      : p(p), i(i) {}

      BAD(hd,inline,noalias)
      constexpr const_iterator(const const_iterator & rhs) noexcept
      : p(rhs.p), i(rhs.i) {}

      BAD(hd,inline,noalias)
      constexpr const_iterator(const_iterator && rhs) noexcept
      : p(std::move(rhs.p)), i(std::move(rhs.i)) {}

      BAD(reinitializes,hd,inline,noalias)
      const_iterator & operator =(const_iterator rhs) noexcept {
        p = rhs.p;
        i = rhs.i;
        return *this;
      }

      BAD(reinitializes,hd,inline,noalias)
      const_iterator & operator =(const_iterator && rhs) noexcept {
        p = std::move(rhs.p);
        i = std::move(rhs.i);
        return *this;
      }

      // default constructible, moveable
      T const * p;
      ptrdiff_t i;

      // NB: iterators are only comparable if they come from the same container
      BAD(hd,inline,pure)
      friend bool operator ==(const_iterator lhs, const_iterator rhs) noexcept {
        return lhs.i == rhs.i;
      }

      // NB: iterators are only comparable if they come from the same container
      BAD(hd,inline,pure)
      friend bool operator !=(const_iterator lhs, const_iterator rhs) noexcept {
        return lhs.i != rhs.i;
      }

      BAD(hd,inline,pure)
      friend bool operator <(const_iterator lhs, const_iterator rhs) noexcept {
        return lhs.i < rhs.i;
      }

      BAD(hd,inline,pure)
      friend bool operator >(const_iterator lhs, const_iterator rhs) noexcept {
        return lhs.i > rhs.i;
      }

      BAD(hd,inline,pure)
      friend bool operator <=(const_iterator lhs, const_iterator rhs) noexcept {
        return lhs.i <= rhs.i;
      }

      BAD(hd,inline,pure)
      friend bool operator >=(const_iterator lhs, const_iterator rhs) noexcept {
        return lhs.i >= rhs.i;
      }

      BAD(hd,inline,noalias)
      const_iterator & operator ++() noexcept {
        ++i;
        return *this;
      }

      BAD(hd,inline,noalias)
      const_iterator operator ++(int) noexcept {
        return { p, i++ };
      }

      BAD(hd,inline,noalias)
      const_iterator & operator --() noexcept {
        --i;
        return *this;
      }

      BAD(hd,inline,noalias)
      const_iterator operator --(int) noexcept {
        return { p, i-- };
      }

      BAD(hd,inline,pure)
      friend const_iterator operator +(const_iterator lhs, ptrdiff_t rhs) noexcept {
        return { lhs.p, lhs.i + rhs };
      }

      friend const_iterator operator +(ptrdiff_t lhs, const_iterator & rhs) noexcept {
        return { rhs.p, rhs.i + lhs };
      }

      BAD(hd,inline,pure)
      friend const_iterator operator -(const_iterator lhs, ptrdiff_t rhs) noexcept {
        return { lhs.p, lhs.i - rhs };
      }

      BAD(hd,inline,pure)
      friend ptrdiff_t operator -(const_iterator lhs, const_iterator rhs) noexcept {
        assert(lhs.p == rhs.p);
        return lhs.i - rhs.i;
      }

      BAD(hd,inline,noalias)
      const_iterator & operator +=(ptrdiff_t rhs) const noexcept {
        i += rhs;
        return *this;
      }

      BAD(hd,inline,noalias)
      const_iterator & operator -=(ptrdiff_t rhs) const noexcept {
        i -= rhs;
        return *this;
      }

      BAD(hd,inline,pure)
      reference operator *() const noexcept {
        return *reinterpret_cast<pointer>(p + i*s);
      }

      BAD(hd,inline,pure)
      pointer operator ->() const noexcept {
        return reinterpret_cast<pointer>(p + i*s);
      }

      BAD(hd,inline,pure)
      reference operator[](difference_type di) const noexcept {
        return *reinterpret_cast<pointer>(p + (i+di)*s);
      }

      BAD(hd,inline,pure)
      bool valid() const noexcept {
        return p != nullptr && 0 <= i && i < d;
      }
    };

    struct iterator {
      using value_type = plane;
      using difference_type = ptrdiff_t;
      using pointer = value_type *;
      using reference = value_type &;
      using iterator_category = std::random_access_iterator_tag;

      BAD(hd,inline,noalias) constexpr
      explicit iterator(
        T* p = nullptr,
        difference_type i = 0
      ) noexcept
      : p(p), i(i) {}

      BAD(hd,inline,noalias) constexpr
      iterator(const iterator & rhs) noexcept
      : p(rhs.p), i(rhs.i) {}

      BAD(hd,inline,noalias) constexpr
      iterator(iterator && rhs) noexcept
      : p(std::move(rhs.p)), i(std::move(rhs.i)) {}

      BAD(reinitializes,hd,inline,noalias)
      iterator & operator =(iterator rhs) noexcept {
        p = rhs.p;
        i = rhs.i;
        return *this;
      }

      BAD(reinitializes,hd,inline,noalias)
      iterator & operator =(iterator && rhs) noexcept {
        p = std::move(rhs.p);
        i = std::move(rhs.i);
        return *this;
      }

      T * p;
      difference_type i;

      BAD(hd,inline,pure)
      operator const_iterator() const {
        return const_iterator(p, i);
      }

      // valid across sources
      BAD(hd,inline,pure)
      friend bool operator ==(iterator lhs, iterator rhs) noexcept {
        return lhs.i == rhs.i;
      }

      BAD(hd,inline,pure)
      friend bool operator !=(iterator lhs, iterator rhs) noexcept {
        return lhs.i != rhs.i;
      }

      // valid within a single source
      BAD(hd,inline,pure)
      friend bool operator <(iterator lhs, iterator rhs) noexcept {
        return lhs.i < rhs.i;
      }

      BAD(hd,inline,pure)
      friend bool operator >(iterator lhs, iterator rhs) noexcept {
        return lhs.i > rhs.i;
      }

      BAD(hd,inline,pure)
      friend bool operator <=(iterator lhs, iterator rhs) noexcept {
        return lhs.i <= rhs.i;
      }

      BAD(hd,inline,pure)
      friend bool operator >=(iterator lhs, iterator rhs) noexcept {
        return lhs.i >= rhs.i;
      }

      BAD(hd,inline,noalias)
      iterator & operator ++() noexcept {
        ++i;
        return *this;
      }

      BAD(hd,inline,noalias)
      iterator operator ++(int) noexcept {
        return iterator(p, i++);
      }

      BAD(hd,inline,noalias)
      iterator & operator --() noexcept {
        --i;
        return *this;
      }

      BAD(hd,inline,noalias)
      iterator operator --(int) noexcept {
        return iterator(p, i--);
      }

      BAD(hd,inline,pure)
      friend iterator operator +(iterator lhs, ptrdiff_t rhs) noexcept {
        return iterator(lhs.p, lhs.i + rhs);
      }

      BAD(hd,inline,pure)
      friend iterator operator -(iterator lhs, ptrdiff_t rhs) noexcept {
        return iterator(lhs.p, lhs.i - rhs);
      }

      BAD(hd,inline,pure)
      friend ptrdiff_t operator -(iterator lhs, iterator rhs) noexcept {
        assert(lhs.p == rhs.p);
        return lhs.i - rhs.i;
      }

      BAD(hd,inline,noalias)
      iterator & operator +=(ptrdiff_t rhs) const noexcept {
        i += rhs;
        return *this;
      }

      BAD(hd,inline,noalias)
      iterator & operator -=(ptrdiff_t rhs) const noexcept {
        i -= rhs;
        return *this;
      }

      BAD(hd,inline,pure)
      reference operator *() const noexcept {
        return *reinterpret_cast<pointer>(p + i*s);
      }

      BAD(hd,inline,pure)
      pointer operator ->() const noexcept {
        return reinterpret_cast<pointer>(p + i*s);
      }

      BAD(hd,inline,pure)
      reference operator[](ptrdiff_t di) const noexcept {
        return *reinterpret_cast<pointer>(p + (i+di)*s);
      }

      BAD(hd,inline,pure)
      bool valid() const noexcept {
        return 0 <= i && i < d;
      }

      friend iterator operator + (ptrdiff_t lhs, iterator & rhs) {
        return iterator(rhs.p, rhs.i + lhs);
      }
    };

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    BAD(hd,inline,const)
    iterator begin() noexcept {
      return iterator(data + delta, 0);
    }

    BAD(hd,inline,const)
    iterator end() noexcept {
      return iterator(data + delta, d);
    }

    BAD(hd,inline,const)
    reverse_iterator rbegin() noexcept {
      return reverse_iterator(iterator(data + delta, d-1));
    }

    BAD(hd,inline,const)
    reverse_iterator rend() noexcept {
      return reverse_iterator(iterator(data + delta, -1));
    }

    BAD(hd,inline,const)
    const_iterator begin() const noexcept {
      return const_iterator(data + delta, 0);
    }

    BAD(hd,inline,const)
    const_iterator end() const noexcept {
      return const_iterator(data + delta, d);
    }

    BAD(hd,inline,const)
    const_iterator cbegin() const noexcept {
      return const_iterator(data + delta, 0);
    }

    BAD(hd,inline,const)
    const_iterator cend() const noexcept {
      return const_iterator(data + delta, d);
    }

    BAD(hd,inline,const)
    const_reverse_iterator rbegin() const noexcept {
      return reverse_iterator(const_iterator(data + delta, d-1));
    }

    BAD(hd,inline,const)
    const_reverse_iterator rend() const noexcept {
      return reverse_iterator(const_iterator(data + delta, -1));
    }

    BAD(hd,inline,const)
    const_reverse_iterator crbegin() const noexcept {
      return reverse_iterator(const_iterator(data + delta, d-1));
    }

    BAD(hd,inline,const)
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
  }; // type

  // scalar initialization
  template <class T>
  store(const T &) -> store<T,seq<>,sseq<>>;

  // copy stride if copying from another store
  template <class T, class dim, class stride>
  store(const store<T,dim,stride> &) -> store<T,dim,stride>;

  // when fed one store_expression copy its dimensions, and pick a row_major order
  template <class B, size_t d, size_t... ds>
  store(const store_expr<B,d,ds...> &) -> store<typename B::element,seq<d,ds...>>; // implicitly row_major

  // when constructed from multiple storage expressions, use the length of the argument list to build the outermost dimension.
  template <class B, size_t d, size_t...ds, class... U>
  store(const store_expr<B,d,ds...> &, U...) -> store<typename B::element,seq<1+sizeof...(U),d,ds...>>;

  // vector construction
  template <class T, class... U>
  store(T, U...) -> store<T,seq<1+sizeof...(U)>>;

  template <class T, size_t N>
  using vec = store<T,seq<N>>;

  // for a more general version of the multiple storage expression rule, we'd need to be able to know dimensions for arbitrary types.
  // if we had something like:
  // dim<float> = seq<>, dim<store<T,Stride,Dim>> = Stride
  // dim<store_expr<B,d,ds...>> = seq<d,ds...>
  // base_type<B> to get at B::element as well
  // we could make this more robust
  // take shape and type from first arg, 
 
  

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
  template <size_t d, class T>
  BAD(hd,inline)
  auto rep(T t) noexcept -> store<T,seq<d>,sseq<0>> {
    return t;
  }

  // used to show the values in an expr or fixed array
  template <size_t d, class T>
  struct show_values {
    T const & data;

    BAD(hd)
    show_values(BAD(lifetimebound) const T & data) noexcept
    : data(data) {};

    BAD(hd)
    show_values(BAD(lifetimebound) const T (&data)[d]) noexcept
    : data(data) {}
  };

  template <size_t d, class T>
  BAD(hd)
  std::ostream & operator << (std::ostream &os, const show_values<d,T> & rhs) {
    os << "{";
    for (size_t i=0;i<d;++i) {
      if (i) os << ",";
      os << rhs.data[i];
    }
    return os << "}";
  }

  // CTAD
  template <size_t d, size_t... ds, class B>
  show_values(store_expr<B,d,ds...> const & data)
    -> show_values<d, store_expr<B,d,ds...>>;

  // CTAD
  template <size_t d, class T>
  show_values(T(&)[d])
    -> show_values<d,T*>;

  template <class L, class R, size_t d, size_t... ds>
  BAD(hd,inline,const)
  auto operator +(
    BAD(lifetimebound) store_expr<L,d,ds...> const &l,
    BAD(lifetimebound) store_expr<R,d,ds...> const &r
  ) noexcept {
    return store_add_expr<L,R,d,ds...>(l(),r());
  }
}
