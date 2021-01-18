#pragma once

#include "shape.h"

namespace bad {
  namespace detail {
    template <typename T, typename Dim, typename Stride = row_major<Dim>>
    struct store_{};

    template <typename T, typename Dim, typename Stride = row_major<Dim>>
    struct store_t{};

    template <typename T>
    struct store_t<T,seq<>,seq<>> {
      using type = T;
    };

    template <typename T, typename Dim, typename Stride> {
    struct store_t<T,Dim,Stride> {
      using type = store_<T,Dim,Stride>;
    };
  }

  template <typename T, typename Dim, typename Stride>
  using store = typename detail::store_t<T,Dim,Stride>::type;

  namespace detail {
    template <typename T, typename Dim, typename Stride = row_major<Dim>>
    struct const_cursor
    : std::iterator<
        random_access_iterator_tag,
        store<T,seq_tail<Dim>,seq_tail<Stride>> const,
        typename std::make_signed<seq_element_type<Dim>>
      > {
      static constexpr auto D = seq_head<Dim>;
      static constexpr auto S = seq_head<Stride>;

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
      diff_t operator - (const_cursor const & rhs) const noexcept {
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
        random_access_iterator_tag,
        store<T,seq_tail<Dim>,seq_tail<Stride>>,
        typename std::make_signed<seq_element_type<Dim>>::type
      > {
      static constexpr auto D = seq_head<Dim>;
      static constexpr auto S = seq_head<Stride>;

      // default constructible, moveable
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
      diff_t d,
      const const_cursor<T,Dim,Stride> & rhs
    ) {
      return rhs + d;
    }

    template <typename T, typename Dim, typename Stride>
    inline cursor<T,Dim,Stride> operator +(
      diff_t d,
      const cursor<T,Dim,Stride> & rhs
    ) {
      return rhs + d;
    }

    template <typename B, typename Dim>
    struct store_expr {
      static constexpr auto D = seq_head<Dim>;
      using difference_type = typename std::make_signed<seq_element_type<Dim>>::type;
      using index_type = typename std::make_unsigned<seq_element_type<Dim>>::type;
      auto operator [](index_type i) const noexcept {
        return static_cast<B const &>(*this)[i];
      }
    };

    template <typename L, typename R, typename Dim>
    struct store_add_expr : store_expr<store_add_expr<L,R,Dim>,Dim> {
      L const & l;
      R const & r;
      auto operator [](index_type i) const noexcept {
        return l[i] + r[i];
      }
    };

    template <typename L, typename R, typename Dim>
    auto operator+(
      store_expr<L,Dim> const &u,
      store_expr<R,Dim> const &v
    ) {
      return store_add_expr<L,R,Dim>(
        *static_cast<L const *>(&l),
        *static_cast<R const *>(&r)
      );
    }

    // a lens is a pointer into a matrix, not a matrix
    template <typename T, typename Dim, typename Stride>
    struct store_ : public store_expr<store_<T,Dim,Stride>,Dim> {
      using iterator               = cursor<T,Dim,Stride>;
      using const_iterator         = const_cursor<T,Dim,Stride>;
      using reverse_iterator       = std::reverse_iterator<iterator>;
      using const_reverse_iterator = std::reverse_iterator<const_iterator>;
      using plane = store<T,seq_tail<Dim>,seq_tail<Stride>>; // note store, not store_
      static constexpr auto S = seq_head<Stride>;

      store_() : data() {}

      // this requires me to know that dimensions are dense and exhaustive
      // template <typename = std::enable_if_t<std::is_same_v<sort<Stride>,row_major<Dim>>
      // store_(const store_ & rhs) : data(rhs.data) {}

      template <typename B>
      store_(store_expr<B,Dim> const & rhs) {
        for (int i=0;i<D;++i)
          at(i) = rhs[i];
      }

      T data[D];

      iterator begin() noexcept                       { return { data, 0 }; }
      iterator end() noexcept                         { return { data, D }; }
      reverse_iterator rbegin() noexcept              { return reverse_iterator(cursor{data, D-1}); }
      reverse_iterator rend() noexcept                { return reverse_iterator(cursor{data, -1 }); }
      const_iterator begin() const noexcept           { return { data, 0 }; }
      const_iterator end() const noexcept             { return { data, D }; }
      const_iterator cbegin() const noexcept          { return { data, 0 }; }
      const_iterator cend() const noexcept            { return { data, D }; }
      const_reverse_iterator rbegin() const noexcept  { return reverse_iterator(cursor{data, D-1 }); }
      const_reverse_iterator rend() const noexcept    { return reverse_iterator(cursor{data, -1 }); }
      const_reverse_iterator crbegin() const noexcept { return reverse_iterator(cursor{data, D-1 }); }
      const_reverse_iterator crend() const noexcept   { return reverse_iterator(cursor{ data, -1 }); }

      plane & at(index_type i) noexcept {
        return *reinterpret_cast<plane *>(data + i*S);
      }

      plane const & at(index_type i) const noexcept {
        return *reinterpret_cast<plane const *>(data + i*S);
      }

      plane & operator[](index_type i) noexcept {
        return *reinterpret_cast<plane *>(data + i*S);
      }

      plane const & operator[](index_type i) const noexcept {
        return *reinterpret_cast<plane const *>(data + i*S);
      }

      template <typename B>
      store & operator = (store_expr<B,Dim> const & rhs) {
        for (int i=0;i<D;++i)
          at(i) = rhs[i];
        return *this;
      }

      template <typename B>
      store & operator += (store_expr<B,Dim> const & rhs) {
        for (int i=0;i<D;++i)
          at(i) += rhs[i];
        return *this;
      }

      template <typename B>
      store & operator -= (store_expr<B,Dim> const & rhs) {
        for (int i=0;i<D;++i)
          at(i) += rhs[i];
        return *this;
      }

      template <typename B>
      store & operator *= (store_expr<B,Dim> const & rhs) {
        for (int i=0;i<D;++i)
          at(i) *= rhs[i];
        return *this;
      }
    };
  }
}