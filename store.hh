#pragma once
#include <array>
#include "seq.hh"

namespace bad {
  namespace detail {
    template <typename T, typename Dim, typename Stride = row_major<Dim>>
    struct store_;

    template <typename T, typename Dim, typename Stride = row_major<Dim>>
    struct store_t{};

    template <typename T, typename Td, typename Ts>
    struct store_t<T,std::integer_sequence<Td>,std::integer_sequence<Ts>> {
      using type = T;
    };

    template <typename T, typename Dim, typename Ts, Ts ... Ss>
    struct store_t<T,Dim,std::integer_sequence<Ts,Ss...>> {
      using type = store_<T,Dim,std::integer_sequence<Ts,Ss...>>;
    };
  } // namespace detail
}
namespace bad {
  template <typename T, typename Dim, typename Stride = row_major<Dim>>
  using store = typename detail::store_t<T,Dim,Stride>::type;

  namespace detail {
    template <typename T, typename Dim, typename Stride = row_major<Dim>>
    struct const_cursor
    : std::iterator<
        std::random_access_iterator_tag,
        store<T,seq_tail<Dim>,seq_tail<Stride>> const,
        typename std::make_signed<seq_element_type<Dim>>::type
      > {
      using super = std::iterator<
        std::random_access_iterator_tag,
        store<T,seq_tail<Dim>,seq_tail<Stride>> const,
        typename std::make_signed<seq_element_type<Dim>>::type
      >;
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

    template <typename T, typename Dim, typename Stride = row_major<Dim>>
    struct cursor
    : std::iterator<
        std::random_access_iterator_tag,
        store<T,seq_tail<Dim>,seq_tail<Stride>>,
        typename std::make_signed<seq_element_type<Dim>>::type
      > {
      using super = std::iterator<
        std::random_access_iterator_tag,
        store<T,seq_tail<Dim>,seq_tail<Stride>>,
        typename std::make_signed<seq_element_type<Dim>>::type
      >;
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

    template <typename T, typename Dim, typename Stride>
    inline const_cursor<T,Dim,Stride> operator +(
      typename cursor<T,Dim,Stride>::difference_type d,
      const const_cursor<T,Dim,Stride> & rhs
    ) {
      return rhs + d;
    }

    template <typename T, typename Dim, typename Stride>
    inline cursor<T,Dim,Stride> operator +(
      typename cursor<T,Dim,Stride>::difference_type d,
      const cursor<T,Dim,Stride> & rhs
    ) {
      return rhs + d;
    }

    template <typename B, typename Dim>
    struct store_expr {
      static constexpr auto D = seq_head<Dim>;
      using index_type = typename std::make_unsigned<seq_element_type<Dim>>::type;
      auto operator [](index_type i) const noexcept {
        return static_cast<B const &>(*this)[i];
      }
    };

    template <typename L, typename R, typename Dim>
    struct store_add_expr : store_expr<store_add_expr<L,R,Dim>,Dim> {
      using index_type = typename std::make_unsigned<seq_element_type<Dim>>::type;

      L const & l;
      R const & r;

      store_add_expr(store_expr<L,Dim> const & l, store_expr<R,Dim> const & r)
      : l(static_cast<L const &>(l)), r(static_cast<R const &>(r)) {}
      auto operator [](index_type i) const noexcept {
        return l[i] + r[i];
      }
    };

    template <typename L, typename R, typename Dim>
    auto operator+(store_expr<L,Dim> const &l, store_expr<R,Dim> const &r) {
      return store_add_expr<L,R,Dim>(l,r);
    }

    template <typename L, typename R, typename Dim>
    struct store_sub_expr : store_expr<store_sub_expr<L,R,Dim>,Dim> {
      using index_type = typename std::make_unsigned<seq_element_type<Dim>>::type;

      L const & l;
      R const & r;

      store_sub_expr(store_expr<L,Dim> const & l, store_expr<R,Dim> const & r)
      : l(static_cast<L const &>(l)), r(static_cast<R const &>(r)) {}
      auto operator [](index_type i) const noexcept {
        return l[i] - r[i];
      }
    };

    template <typename L, typename R, typename Dim>
    auto operator-(store_expr<L,Dim> const &l, store_expr<R,Dim> const &r) {
      return store_sub_expr<L,R,Dim>(l,r);
    }

    template <typename L, typename R, typename U, U D, U ... Ds>
    constexpr bool operator==(store_expr<L,std::integer_sequence<U,D,Ds...>> const &l, store_expr<R,std::integer_sequence<U,D,Ds...>> const & r) noexcept {
      for (U i=0;i<D;++i) {
        if (l[i] != r[i]) return false;
      }
      return true;
    }

    template <typename L, typename R, typename U, U D, U ... Ds>
    constexpr bool operator!=(store_expr<L,std::integer_sequence<U,D,Ds...>> const &l, store_expr<R,std::integer_sequence<U,D,Ds...>> const & r) noexcept {
      for (U i=0;i<D;++i) {
        if (l[i] != r[i]) return true;
      }
      return false;
    }
  }

  namespace detail {
    // a lens is a pointer into a matrix, not a matrix
    template <typename T, typename Dim, typename Stride>
    struct store_ : public store_expr<store_<T,Dim,Stride>,Dim> {
      using index_type             = typename std::make_unsigned<seq_element_type<Dim>>::type;
      using iterator               = cursor<T,Dim,Stride>;
      using const_iterator         = const_cursor<T,Dim,Stride>;
      using reverse_iterator       = std::reverse_iterator<iterator>;
      using const_reverse_iterator = std::reverse_iterator<const_iterator>;
      using plane                  = store<T,seq_tail<Dim>,seq_tail<Stride>>; // note store, not store_

      static constexpr auto D = seq_head<Dim>;
      static constexpr auto S = seq_head<Stride>;

      constexpr store_() : data() {}

      constexpr store_(const T & value) : data() {
        std::fill(begin(),end(),value);
      }

      constexpr store_(std::initializer_list<T> list) : data() {
        assert(list.size() <= D);
        std::copy(list.begin(),list.end(),begin());
      }


//      store_(T value = 0) : data() {
        //for (index_type i=0;i<D;++i)
        //  at(i) = value;
  //    }

      // this requires me to know that dimensions are dense and exhaustive
      // template <typename = std::enable_if_t<std::is_same_v<sort<Stride>,row_major<Dim>>
      // store_(const store_ & rhs) : data(rhs.data) {}

      template <typename B>
      constexpr store_(store_expr<B,Dim> const & rhs) {
        for (index_type i=0;i<D;++i)
          at(i) = rhs[i];
      }

      T data[seq_prod<Dim>];

      store_ & operator =(T value) {
        for (index_type i=0;i<D;++i)
          at(i) = value;
        return *this;
      }

      store_ & operator =(std::initializer_list<T> list) {
        assert(list.size()<=D);
        std::copy(list.begin(),list.end(),begin());
        return *this;
      }

      iterator begin() noexcept                       { return iterator(data, 0); }
      iterator end() noexcept                         { return iterator(data, D); }
      reverse_iterator rbegin() noexcept              { return reverse_iterator(iterator(data, D-1)); }
      reverse_iterator rend() noexcept                { return reverse_iterator(iterator(data, -1)); }
      const_iterator begin() const noexcept           { return const_iterator(data, 0); }
      const_iterator end() const noexcept             { return const_iterator(data, D); }
      const_iterator cbegin() const noexcept          { return const_iterator(data, 0); }
      const_iterator cend() const noexcept            { return const_iterator(data, D); }
      const_reverse_iterator rbegin() const noexcept  { return reverse_iterator(const_iterator(data, D-1)); }
      const_reverse_iterator rend() const noexcept    { return reverse_iterator(const_iterator(data, -1)); }
      const_reverse_iterator crbegin() const noexcept { return reverse_iterator(const_iterator(data, D-1)); }
      const_reverse_iterator crend() const noexcept   { return reverse_iterator(const_iterator(data, -1)); }

      plane & at(index_type i) noexcept {
        return reinterpret_cast<plane &>(data[i*S]);
      }

      plane const & at(index_type i) const noexcept {
        return reinterpret_cast<plane const &>(data[i*S]);
      }

      plane & operator[](index_type i) noexcept {
        return reinterpret_cast<plane &>(data[i*S]);
      }

      plane const & operator[](index_type i) const noexcept {
        return reinterpret_cast<plane const &>(data[i*S]);
      }

      template <typename B>
      store_ & operator = (store_expr<B,Dim> const & rhs) {
        for (int i=0;i<D;++i)
          at(i) = rhs[i];
        return *this;
      }

      template <typename B>
      store_ & operator += (store_expr<B,Dim> const & rhs) {
        for (int i=0;i<D;++i)
          at(i) += rhs[i];
        return *this;
      }

      template <typename B>
      store_ & operator -= (store_expr<B,Dim> const & rhs) {
        for (int i=0;i<D;++i)
          at(i) -= rhs[i];
        return *this;
      }

      template <typename B>
      store_ & operator *= (store_expr<B,Dim> const & rhs) {
        for (int i=0;i<D;++i)
          at(i) *= rhs[i];
        return *this;
      }

      // can we use trickery to superimpose this as '.t' with no ()'s?

      // sfinae?
      //using transpose = store_<T,seq_transpose<Dim>,seq_transpose<Stride>>;
      //transpose & t() { return reinterpret_cast<transpose &>(*this); }
      //const transpose & t() const { return reinterpret_cast<transpose &>(*this); }
    };

    template<typename T, typename Dim, typename Stride>
    std::ostream &operator <<(std::ostream &os, const store_<T,Dim,Stride> & rhs) {
      // emitting a square vector.
      os << "{";
      auto i = rhs.begin();
      while (i != rhs.end()) {
        os << *i;
        ++i;
        if (i == rhs.end()) break;
        os << ",";
      }
      os << "}";
      return os;
    }
  }
}
