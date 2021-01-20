#pragma once
#include <array>
#include "seq.hh"
#include "offset.hh"

namespace bad {
  namespace detail {
    template <class T, class Dim, class Stride = row_major<Dim>>
    struct store_;

    template <class T, class Dim, class Stride = row_major<Dim>>
    struct store_t{};

    template <class T, class Td, class Ts>
    struct store_t<T,seq_t<Td>,seq_t<Ts>> {
      using type = T;
    };

    template <class T, class Dim, class Ts, Ts ... Ss>
    struct store_t<T,Dim,seq_t<Ts,Ss...>> {
      using type = store_<T,Dim,seq_t<Ts,Ss...>>;
    };
  } // namespace detail
}
namespace bad {
  template <class T, class Dim, class Stride = row_major<Dim>>
  using store = typename detail::store_t<T,Dim,Stride>::type;

  namespace detail {
    template <class T, class Dim, class Stride = row_major<Dim>>
    struct const_cursor
    : std::iterator<
        std::random_access_iterator_tag,
        store<T,seq_tail<Dim>,seq_tail<Stride>> const,
        typename std::make_signed<seq_element_type<Dim>>::type
      > {
    private:
      using super = std::iterator<
        std::random_access_iterator_tag,
        store<T,seq_tail<Dim>,seq_tail<Stride>> const,
        typename std::make_signed<seq_element_type<Dim>>::type
      >;
    public:
      using difference_type = typename super::difference_type;
      using pointer = typename super::pointer;
      using reference = typename super::reference;
      using value_type = typename super::value_type;
      using iterator_category = typename super::iterator_category;

      static constexpr auto D = seq_head<Dim>;
      static constexpr auto S = seq_head<Stride>;

      constexpr explicit const_cursor(T const * p = nullptr, difference_type i = 0) noexcept : p(p), i(i) {}
      constexpr const_cursor(const_cursor const & rhs) noexcept : p(rhs.p), i(rhs.i) {}
      constexpr const_cursor(const_cursor && rhs) noexcept : p(std::move(rhs.p)), i(std::move(rhs.i)) {}

      const_cursor & operator =(const_cursor const & rhs) { p = rhs.p; i = rhs.i; return *this; }
      const_cursor & operator =(const_cursor && rhs) { p = std::move(rhs.p); i = std::move(rhs.i); return *this; }

      // default constructible, moveable
      T const * p;
      difference_type i; // signed because rend actually has a negative index

      bool operator == (const_cursor const & rhs) const noexcept { return p == rhs.p && i == rhs.i; }
      bool operator != (const_cursor const & rhs) const noexcept { return p != rhs.p || i != rhs.i; }
      bool operator < (const_cursor const & rhs) const noexcept { return i < rhs.i; }
      bool operator > (const_cursor const & rhs) const noexcept { return i > rhs.i; }
      bool operator <= (const_cursor const & rhs) const noexcept { return i <= rhs.i; }
      bool operator >= (const_cursor const & rhs) const noexcept { return i >= rhs.i; }

      const_cursor & operator ++ () noexcept { ++i; return *this; }
      const_cursor operator ++ (int) noexcept { return { p, i++ }; }
      const_cursor & operator -- () noexcept { --i; return *this; }
      const_cursor operator -- (int) noexcept { return { p, i-- }; }
      const_cursor operator + (difference_type d) const noexcept { return { p, i + d }; }
      const_cursor operator - (difference_type d) const noexcept { return { p, i + d }; }
      difference_type operator - (const_cursor const & rhs) const noexcept {
        assert(p == rhs.p);
        return i - rhs.i;
      }
      const_cursor & operator += (difference_type d) const noexcept { i += d; return *this; }
      const_cursor & operator -= (difference_type d) const noexcept { i -= d; return *this; }

      reference operator *() const noexcept {
        return *reinterpret_cast<pointer>(p + i*S);
      }
      pointer operator ->() const noexcept {
        return reinterpret_cast<pointer>(p + i*S);
      }
      reference operator[](difference_type di) const noexcept {
        return *reinterpret_cast<pointer>(p + (i+di)*S);
      }
      bool valid() const noexcept {
        return p != nullptr && 0 <= i && i <= D;
      }
    };

    template <class T, class Dim, class Stride = row_major<Dim>>
    struct cursor
    : std::iterator<
        std::random_access_iterator_tag,
        store<T,seq_tail<Dim>,seq_tail<Stride>>,
        typename std::make_signed<seq_element_type<Dim>>::type
      > {
    private:
      using super = std::iterator<
        std::random_access_iterator_tag,
        store<T,seq_tail<Dim>,seq_tail<Stride>>,
        typename std::make_signed<seq_element_type<Dim>>::type
      >;
    public:
      using difference_type = typename super::difference_type;
      using pointer = typename super::pointer;
      using reference = typename super::reference;
      using value_type = typename super::value_type;
      using iterator_category = typename super::iterator_category;

      static constexpr auto D = seq_head<Dim>;
      static constexpr auto S = seq_head<Stride>;

      constexpr explicit cursor(T* p = nullptr, difference_type i = 0) noexcept : p(p), i(i) {}
      constexpr cursor(cursor const & rhs) noexcept : p(rhs.p), i(rhs.i) {}
      constexpr cursor(cursor && rhs) noexcept : p(std::move(rhs.p)), i(std::move(rhs.i)) {}

      cursor & operator =(const cursor & rhs) { p = rhs.p; i = rhs.i; return *this; }
      cursor & operator =(cursor && rhs) { p = std::move(rhs.p); i = std::move(rhs.i); return *this; }

      T * p;
      difference_type i;

      operator const_cursor<T, Dim, Stride>() const {
        return { p, i };
      }

      // valid across sources
      bool operator == (const cursor & rhs) const noexcept { return p == rhs.p && i == rhs.i; }
      bool operator != (const cursor & rhs) const noexcept { return p != rhs.p || i != rhs.i; }

      // valid within a single source
      bool operator < (const cursor & rhs) const noexcept { return i < rhs.i; }
      bool operator > (const cursor & rhs) const noexcept { return i > rhs.i; }
      bool operator <= (const cursor & rhs) const noexcept { return i <= rhs.i; }
      bool operator >= (const cursor & rhs) const noexcept { return i >= rhs.i; }

      cursor & operator ++ () noexcept { ++i; return *this; }
      cursor operator ++ (int) noexcept { return { p, i++ }; }
      cursor & operator -- () noexcept { --i; return *this; }
      cursor operator -- (int) noexcept { return { p, i-- }; }
      cursor operator + (difference_type d) const noexcept { return { p, i + d }; }
      cursor operator - (difference_type d) const noexcept { return { p, i + d }; }

      difference_type operator - (const cursor & rhs) const noexcept {
        assert(p == rhs.p);
        return i - rhs.i;
      }

      cursor & operator += (difference_type d) const noexcept { i += d; return *this; }
      cursor & operator -= (difference_type d) const noexcept { i -= d; return *this; }

      reference operator *() const noexcept {
        return *reinterpret_cast<pointer>(p + i*S);
      }

      pointer operator ->() const noexcept {
        return reinterpret_cast<pointer>(p + i*S);
      }

      reference operator[](difference_type di) const noexcept {
        return *reinterpret_cast<pointer>(p + (i+di)*S);
      }

      bool valid() const noexcept {
        return 0 <= i && i <= D;
      }
    };

    template <class T, class Dim, class Stride>
    inline const_cursor<T,Dim,Stride> operator +(
      typename cursor<T,Dim,Stride>::difference_type d,
      const const_cursor<T,Dim,Stride> & rhs
    ) {
      return rhs + d;
    }

    template <class T, class Dim, class Stride>
    inline cursor<T,Dim,Stride> operator +(
      typename cursor<T,Dim,Stride>::difference_type d,
      const cursor<T,Dim,Stride> & rhs
    ) {
      return rhs + d;
    }
  }

  template <size_t N, class B> auto pull(B & rhs, typename B::index_type i) noexcept {
    return rhs.template pull<N>(i);
  }
  template <size_t N, class B> auto pull(const B & rhs, typename B::index_type i) noexcept {
    return rhs.template pull<N>(i);
  }

  namespace detail {

    template <class B, class Dim>
    struct store_expr {
      static constexpr auto D = seq_head<Dim>;
      using index_type = typename std::make_unsigned<seq_element_type<Dim>>::type;

      store_expr const & operator()() const noexcept { return *this; }
      template <class... Args> auto operator ()(index_type i, Args... args) const noexcept {
        return (*this)[i](args...);
      }

      auto operator [](index_type i) const noexcept {
        return static_cast<B const &>(*this)[i];
      }

      template <size_t N> auto pull(index_type i) const noexcept {
        return static_cast<B const &>(*this).template pull<N>(i);
      }

      template <size_t D> auto rep() const noexcept {
        return static_cast<B const &>(*this).template rep<D>();
      }
    };
  }

  template <auto D, class B, class Dim> auto rep(detail::store_expr<B,Dim> const & base) noexcept {
    return base.template rep<D>();
  }

  namespace detail {
    // repeat n times.
    template <auto D, class B, class Dim>
    struct store_rep_expr : store_expr<store_rep_expr<D,B,Dim>, seq_cons<D,Dim>> {
      B const & base;
      using index_type = typename std::make_unsigned<decltype(D)>::type;

      store_rep_expr(store_expr<B,Dim> const & base) noexcept
      : base(static_cast<B const &>(base)) {}

      auto operator [](index_type i) const noexcept {
        return base;
      }
      template <size_t N> auto pull(index_type i) const noexcept {
        if constexpr(N == 0) {
          return base;
        } else {
          return pull<N-1>(i).template rep<D>();
        }
      }
      template <size_t E> auto rep() const noexcept {
        return store_rep_expr<E, store_rep_expr<D,B,Dim>, Dim>(*this);
      }
    };

  }


  namespace detail {
    template <class L, class R, class Dim>
    struct store_add_expr : store_expr<store_add_expr<L,R,Dim>,Dim> {
      using index_type = typename std::make_unsigned<seq_element_type<Dim>>::type;

      L const & l;
      R const & r;

      store_add_expr(store_expr<L,Dim> const & l, store_expr<R,Dim> const & r) noexcept
      : l(static_cast<L const &>(l)), r(static_cast<R const &>(r)) {}

      auto operator [](index_type i) const noexcept {
        return l[i] + r[i];
      }
      template <size_t N> auto pull(index_type i) const noexcept {
        return l.template pull<N>(i) + r.template pull<N>(i);
      }
      template <seq_element_type<Dim> D> auto rep() const noexcept {
        return store_rep_expr<D, store_add_expr<L,R,Dim>, Dim>(*this);
      }
    };

    template <class L, class R, class Dim>
    auto operator+(store_expr<L,Dim> const &l, store_expr<R,Dim> const &r) noexcept {
      return store_add_expr<L,R,Dim>(l,r);
    }

    template <class L, class R, class Dim>
    struct store_sub_expr : store_expr<store_sub_expr<L,R,Dim>,Dim> {
      using index_type = typename std::make_unsigned<seq_element_type<Dim>>::type;

      L const & l;
      R const & r;

      store_sub_expr(store_expr<L,Dim> const & l, store_expr<R,Dim> const & r) noexcept
      : l(static_cast<L const &>(l)), r(static_cast<R const &>(r)) {}
      auto operator [](index_type i) const noexcept {
        return l[i] - r[i];
      }
      template <size_t N> auto pull(index_type i) const noexcept {
        return l.template pull<N>(i) - r.template pull<N>(i);
      }
      template <seq_element_type<Dim> D> auto rep() const noexcept {
        return store_rep_expr<D, store_add_expr<L,R,Dim>, Dim>(*this);
      }
    };

    template <class L, class R, class Dim>
    auto operator-(store_expr<L,Dim> const &l, store_expr<R,Dim> const &r) noexcept {
      return store_sub_expr<L,R,Dim>(l,r);
    }

    template <class L, class R, class Dim>
    struct store_hadamard_expr : store_expr<store_hadamard_expr<L,R,Dim>,Dim> {
      using index_type = typename std::make_unsigned<seq_element_type<Dim>>::type;

      L const & l;
      R const & r;

      store_hadamard_expr(store_expr<L,Dim> const & l, store_expr<R,Dim> const & r) noexcept
      : l(static_cast<L const &>(l)), r(static_cast<R const &>(r)) {}
      auto operator [](index_type i) const noexcept {
        return l[i] - r[i];
      }
      template <size_t N> auto pull(index_type i) const noexcept {
        return l.template pull<N>(i) * r.template pull<N>(i);
      }
      template <seq_element_type<Dim> D> auto rep() const noexcept {
        return store_rep_expr<D, store_add_expr<L,R,Dim>, Dim>(*this);
      }
    };

    // NOTE: multiplication is hadamard by default, not matrix multiplication or matrix vector!!!
    template <class L, class R, class Dim>
    auto operator*(store_expr<L,Dim> const &l, store_expr<R,Dim> const &r) noexcept {
      return store_hadamard_expr<L,R,Dim>(l,r);
    }

    template <class L, class R, class U, U D, U... Ds>
    constexpr bool operator==(store_expr<L,seq_t<U,D,Ds...>> const &l, store_expr<R,seq_t<U,D,Ds...>> const & r) noexcept {
      for (U i=0;i<D;++i) {
        if (l[i] != r[i]) return false;
      }
      return true;
    }

    template <class L, class R, class U, U D, U... Ds>
    constexpr bool operator!=(store_expr<L,seq_t<U,D,Ds...>> const &l, store_expr<R,seq_t<U,D,Ds...>> const & r) noexcept {
      for (U i=0;i<D;++i) {
        if (l[i] != r[i]) return true;
      }
      return false;
    }
  }

  template <size_t N, class L> using seq_pull = seq_cons<seq_nth<N,L>,seq_skip_nth<N,L>>;

  namespace detail {

    // a lens is a pointer into a matrix, not a matrix
    template <class T, class Dim, class Stride>
    struct store_ : public store_expr<store_<T,Dim,Stride>,Dim> {
      using index_type = typename std::make_unsigned<seq_element_type<Dim>>::type;
      using iterator = cursor<T,Dim,Stride>;
      using const_iterator = const_cursor<T,Dim,Stride>;
      using reverse_iterator = std::reverse_iterator<iterator>;
      using const_reverse_iterator = std::reverse_iterator<const_iterator>;
      using plane = store<T,seq_tail<Dim>,seq_tail<Stride>>; // note store, not store_

    private:
      template <class D, D...> struct calc_;
      template <class D> struct calc_<D> {
        template <class S, S...> struct at {
          static constexpr ptrdiff_t max = 0;
          static constexpr ptrdiff_t min = 0;
        };
      };
      template <class D, D d, D ... ds> struct calc_<D,d,ds...> {
        template <class S,S...> struct at;
        template <class S, S s, S ... ss> struct at<S,s,ss...> {
          static constexpr ptrdiff_t max = std::max<ptrdiff_t>(0,s*(d-1)) + calc_<D,ds...>::template at<S,ss...>::max;
          static constexpr ptrdiff_t min = std::min<ptrdiff_t>(0,s*(d-1)) + calc_<D,ds...>::template at<S,ss...>::min;
        };
      };
      using calc      = seq_apply<seq_apply<calc_, Dim>::template at, Stride>;
      using calc_tail = seq_apply<seq_apply<calc_, seq_tail<Dim>>::template at, seq_tail<Stride>>;
    public:

      static constexpr auto D = seq_head<Dim>;
      static constexpr auto S = seq_head<Stride>;

      static constexpr size_t    max_index = size_t(calc::max); // non-negative
      static constexpr ptrdiff_t min_index = calc::min; // non-positive
      static constexpr size_t offset = -calc::min; // positive
      static constexpr size_t tail_offset = -calc_tail::min; // positive
      static constexpr size_t delta_offset = offset - tail_offset; // non-negative
      static constexpr size_t size = offset + max_index + 1; // offset so negative strides remain in bounds to shut up valgrind, etc.

      constexpr store_() noexcept : data() {}

      constexpr store_(const T & value) noexcept : data() {
        std::fill(begin(),end(),value);
      }

      constexpr store_(std::initializer_list<T> list) noexcept : data() {
        assert(list.size() <= D);
        std::copy(list.begin(),list.end(),begin());
      }

      template <class B>
      constexpr store_(store_expr<B,Dim> const & rhs) noexcept {
        for (index_type i=0;i<D;++i)
          at(i) = rhs[i];
      }

      T data[size];

      store_ & operator =(T value) noexcept {
        for (index_type i=0;i<D;++i)
          at(i) = value;
        return *this;
      }

      store_ & operator =(std::initializer_list<T> list) noexcept {
        assert(list.size()<=D);
        std::copy(list.begin(),list.end(),begin());
        return *this;
      }

      iterator begin() noexcept                       { return iterator(data + offset, 0); }
      iterator end() noexcept                         { return iterator(data + offset, D); }
      reverse_iterator rbegin() noexcept              { return reverse_iterator(iterator(data + offset, D-1)); }
      reverse_iterator rend() noexcept                { return reverse_iterator(iterator(data + offset, -1)); }
      const_iterator begin() const noexcept           { return const_iterator(data + offset, 0); }
      const_iterator end() const noexcept             { return const_iterator(data + offset, D); }
      const_iterator cbegin() const noexcept          { return const_iterator(data + offset, 0); }
      const_iterator cend() const noexcept            { return const_iterator(data + offset, D); }
      const_reverse_iterator rbegin() const noexcept  { return reverse_iterator(const_iterator(data + offset, D-1)); }
      const_reverse_iterator rend() const noexcept    { return reverse_iterator(const_iterator(data + offset, -1)); }
      const_reverse_iterator crbegin() const noexcept { return reverse_iterator(const_iterator(data + offset, D-1)); }
      const_reverse_iterator crend() const noexcept   { return reverse_iterator(const_iterator(data + offset, -1)); }

      plane & at(index_type i) noexcept {
        return reinterpret_cast<plane &>(data[delta_offset + i*S]);
      }

      plane const & at(index_type i) const noexcept {
        return reinterpret_cast<plane const &>(data[delta_offset + i*S]);
      }

      plane & operator[](index_type i) noexcept {
        return reinterpret_cast<plane &>(data[delta_offset + i*S]);
      }

      plane const & operator[](index_type i) const noexcept {
        return reinterpret_cast<plane const &>(data[delta_offset + i*S]);
      }

      store_ const & operator()() const noexcept { return *this; }
      template <class... Args> auto const &  operator ()(index_type i, Args... args) const noexcept {
        return (*this)[i](args...);
      }

      store_ & operator()() noexcept { return *this; }
      template <class... Args> auto & operator ()(index_type i, Args... args) noexcept {
        return (*this)[i](args...);
      }

      template <class B>
      store_ & operator = (store_expr<B,Dim> const & rhs) noexcept {
        for (int i=0;i<D;++i)
          at(i) = rhs[i];
        return *this;
      }

      template <class B>
      store_ & operator += (store_expr<B,Dim> const & rhs) noexcept {
        for (int i=0;i<D;++i)
          at(i) += rhs[i];
        return *this;
      }

      template <class B>
      store_ & operator -= (store_expr<B,Dim> const & rhs) noexcept {
        for (int i=0;i<D;++i)
          at(i) -= rhs[i];
        return *this;
      }

      template <class B>
      store_ & operator *= (store_expr<B,Dim> const & rhs) noexcept {
        for (int i=0;i<D;++i)
          at(i) *= rhs[i];
        return *this;
      }

      template <size_t N> using store_pull = store_<T, seq_pull<N,Dim>, seq_pull<N,Stride>>;
      template <size_t N> store_pull<N> & pull() noexcept { return reinterpret_cast<store_pull<N>&>(*this); };
      template <size_t N> const store_pull<N> & pull() const noexcept { return reinterpret_cast<store_pull<N> const &>(*this); };
      template <size_t N> typename store_pull<N>::plane & pull(index_type i) noexcept { return pull<N>()[i]; };
      template <size_t N> typename store_pull<N>::plane const & pull(index_type i) const noexcept { return pull<N>()[i]; }

      template <seq_element_type<Dim> D> auto rep() const noexcept {
        return reinterpret_cast<store_<T,seq_cons<D,Dim>,seq_cons<seq_element_type<Stride>(0),Stride>> const &>(*this);
      }
      template <seq_element_type<Dim> D> auto rep() noexcept {
        return reinterpret_cast<store_<T,seq_cons<D,Dim>,seq_cons<seq_element_type<Stride>(0),Stride>> &>(*this);
      }
    };
  }

  namespace detail {
    template <typename T, typename Dim, typename Stride1, typename Stride2>
    void swap(
      store_<T,Dim,Stride1> & l,
      store_<T,Dim,Stride2> & r
    ) {
      using std::swap;
      using L = store<T,Dim,Stride1>;
      for (typename L::index_type i=0;i<l.D;++i)
        swap(l.at(i),r.at(i));
    }
  }

  using detail::swap;

  namespace detail {
    template<class T, class Dim, class Stride>
    std::ostream &operator <<(std::ostream &os, const store_<T,Dim,Stride> & rhs) {
      // emitting a square vector.
      os << "{";
      auto i = rhs.begin();
      while (i != rhs.end()) {
        os << *i;
        if (++i == rhs.end()) break;
        os << ",";
      }
      os << "}";
      return os;
    }
  }
}
