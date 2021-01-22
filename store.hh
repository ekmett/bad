#pragma once
#include <array>
#include "seq.hh"
#include "attrib.hh"

#pragma STDC FP_CONTRACT ON

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

  template <size_t N, class B>
  BAD(hd,inline)
  auto rep(BAD(lifetimebound) B & rhs) noexcept {
    return rhs.template rep<N>();
  }

  template <size_t N, class B>
  BAD(hd,inline)
  auto rep(BAD(lifetimebound) const B & rhs) noexcept {
    return rhs.template rep<N>();
  }

  template <size_t N, class B>
  BAD(hd,inline)
  auto pull(
    BAD(lifetimebound) B & rhs,
    size_t i
  ) noexcept {
    return rhs.template pull<N>(i);
  }

  template <size_t N, class B>
  BAD(hd,inline)
  auto pull(
    BAD(lifetimebound) const B & rhs,
    size_t i
  ) noexcept {
    return rhs.template pull<N>(i);
  }

  namespace detail {

    template <class B, class Dim>
    struct store_expr {
      static constexpr auto D = seq_head<Dim>;
      using index_type = typename std::make_unsigned<seq_element_type<Dim>>::type;

      BAD(hd,inline,const)
      B const & operator()() const noexcept {
        return static_cast<B const &>(*this);
      }

      template <class... Args>
      BAD(hd,inline,flatten)
      auto operator ()(index_type i, Args... args) const noexcept {
        return (*this)[i](args...);
      }

      BAD(hd,inline,flatten)
      auto operator [](index_type i) const noexcept {
        return static_cast<B const &>(*this)[i];
      }

      template <size_t N>
      BAD(hd,inline,flatten)
      auto pull(index_type i) const noexcept {
        return static_cast<B const &>(*this).template pull<N>(i);
      }

      template <size_t D>
      BAD(hd,inline,flatten)
      auto rep() const noexcept {
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

      BAD(hd,inline,noalias)
      store_rep_expr(
        store_expr<B,Dim> const & base
      ) noexcept
      : base(static_cast<B const &>(base)) {}

      BAD(hd,inline,pure)
      auto operator [](BAD(maybe_unused) index_type i) const noexcept {
        return base;
      }

      template <size_t N>
      BAD(hd,inline,flatten)
      auto pull(index_type i) const noexcept {
        if constexpr(N == 0) {
          return base;
        } else {
          return base.template pull<N-1>(i).template rep<D>();
        }
      }

      template <size_t E>
      BAD(hd,inline,flatten,const)
      auto rep() const noexcept {
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

      BAD(hd,inline,noalias)
      store_add_expr(
        BAD(lifetimebound) store_expr<L,Dim> const & l,
        BAD(lifetimebound) store_expr<R,Dim> const & r
      ) noexcept
      : l(static_cast<L const &>(l))
      , r(static_cast<R const &>(r)) {}

      BAD(hd,inline,flatten)
      auto operator [](index_type i) const noexcept {
        return l[i] + r[i];
      }
      template <size_t N>

      BAD(hd,inline,flatten)
      auto pull(index_type i) const noexcept {
        return l.template pull<N>(i) + r.template pull<N>(i);
      }

      template <seq_element_type<Dim> D>
      BAD(hd,inline,flatten,const)
      auto rep() const noexcept {
        return store_rep_expr<D, store_add_expr<L,R,Dim>, Dim>(*this);
      }
    };

    template <class L, class R, class Dim>
    BAD(hd,inline,const)
    auto operator+(
      BAD(lifetimebound) store_expr<L,Dim> const &l,
      BAD(lifetimebound) store_expr<R,Dim> const &r
    ) noexcept {
      return store_add_expr<L,R,Dim>(l,r);
    }

    template <class L, class R, class Dim>
    struct store_sub_expr : store_expr<store_sub_expr<L,R,Dim>,Dim> {
      L const & l;
      R const & r;

      BAD(hd,inline,noalias)
      store_sub_expr(
        BAD(lifetimebound) store_expr<L,Dim> const & l,
        BAD(lifetimebound) store_expr<R,Dim> const & r
      ) noexcept
      : l(static_cast<L const &>(l))
      , r(static_cast<R const &>(r)) {}

      BAD(hd,inline,flatten)
      auto operator [](size_t i) const noexcept {
        return l[i] - r[i];
      }

      template <size_t N>
      BAD(hd,inline,flatten)
      auto pull(size_t i) const noexcept {
        return l.template pull<N>(i) - r.template pull<N>(i);
      }

      template <size_t D>
      BAD(hd,inline,flatten,const)
      auto rep() const noexcept {
        return store_rep_expr<D, store_sub_expr<L,R,Dim>, Dim>(*this);
      }
    };

    template <class L, class R, class Dim>
    BAD(hd,inline,const) auto operator-(
      BAD(lifetimebound) store_expr<L,Dim> const &l,
      BAD(lifetimebound) store_expr<R,Dim> const &r
    ) noexcept {
      return store_sub_expr<L,R,Dim>(l,r);
    }

    template <class L, class R, class Dim>
    struct store_hadamard_expr : store_expr<store_hadamard_expr<L,R,Dim>,Dim> {
      L const & l;
      R const & r;

      BAD(hd,inline,noalias) store_hadamard_expr(
        BAD(lifetimebound) store_expr<L,Dim> const & l,
        BAD(lifetimebound) store_expr<R,Dim> const & r
      ) noexcept
      : l(static_cast<L const &>(l))
      , r(static_cast<R const &>(r)) {}

      BAD(hd,inline,flatten)
      auto operator [](size_t i) const noexcept {
        return l[i] - r[i];
      }

      template <size_t N>
      BAD(hd,inline,flatten)
      auto pull(size_t i) const noexcept {
        return l.template pull<N>(i) * r.template pull<N>(i);
      }

      template <size_t D>
      BAD(hd,inline,const)
      auto rep() const noexcept {
        return store_rep_expr<D, store_hadamard_expr<L,R,Dim>, Dim>(*this);
      }
    };

    // NOTE: multiplication is hadamard by default, not matrix multiplication or matrix vector!!!
    template <class L, class R, class Dim>
    BAD(hd,inline,const) auto operator*(
      BAD(lifetimebound) store_expr<L,Dim> const &l,
      BAD(lifetimebound) store_expr<R,Dim> const &r
    ) noexcept {
      return store_hadamard_expr<L,R,Dim>(l,r);
    }

    template <class L, class R, size_t D, size_t... Ds>
    BAD(hd,inline)
    constexpr bool operator==(
      store_expr<L,seq<D,Ds...>> const & l,
      store_expr<R,seq<D,Ds...>> const & r
    ) noexcept {
      for (size_t i=0;i<D;++i) {
        if (l[i] != r[i]) return false;
      }
      return true;
    }

    template <class L, class R, size_t D, size_t... Ds>
    BAD(hd,inline)
    constexpr bool operator!=(
      store_expr<L,seq<D,Ds...>> const & l,
      store_expr<R,seq<D,Ds...>> const & r
    ) noexcept {
      for (size_t i=0;i<D;++i) {
        if (l[i] != r[i]) return true;
      }
      return false;
    }
  }

  template <size_t N, class L>
  using seq_pull = seq_cons<seq_nth<N,L>,seq_skip_nth<N,L>>;

  namespace detail {
    template <class T, class Dim, class Stride>
    struct store_;

    // store_<int,seq<1,2,3>,sseq<4,5,1>> -- strides can be negative so use sseq.
    template <class T, size_t d0, ssize_t... ds, ptrdiff_t s0, ptrdiff_t... ss>
    struct store_<T,seq<d0,ds...>,sseq<s0,ss...>>
    : store_expr<store_<T,seq<d0,ds...>,sseq<s0,ss...>>,seq<d0,ds...>> {
      using dim = seq<d0,ds...>;
      using stride = sseq<s0,ss...>;
      using plane = store<T,seq<ds...>,sseq<ss...>>;

    private:
      template <class>
      struct calc_;

      template <size_t... is>
      struct calc_<seq<is...>> {
         template <size_t i>
         static constexpr auto ext = ptrdiff_t(seq_nth<i,stride>)*ptrdiff_t(seq_nth<i,dim>-1);

         static constexpr ptrdiff_t max = (0 + ... + std::max<ptrdiff_t>(0,ext<is>));
         static constexpr ptrdiff_t min = (0 + ... + std::min<ptrdiff_t>(0,ext<is>));
      };
      using calc      = calc_<make_seq<seq_length<dim>>>;
      using calc_tail = calc_<seq_range<size_t(1),seq_length<dim>>>;

    public:

      static constexpr size_t D = d0;
      static constexpr ptrdiff_t S = s0;

      static constexpr size_t max_index = size_t(calc::max); // non-negative
      static constexpr ptrdiff_t min_index = calc::min; // non-positive
      static constexpr size_t offset = -calc::min; // positive
      static constexpr size_t tail_offset = -calc_tail::min; // positive

      // non-negative, could be calculated a bit cheaper by just max(0,S*(1-D)) with appropriate conversions?
      static constexpr size_t delta_offset = offset - tail_offset;

      // offset so negative strides remain in bounds
      static constexpr size_t size = offset + max_index + 1;

      BAD(hd,inline)
      constexpr store_() noexcept
      : data() {}

      BAD(hd,inline)
      constexpr store_(
        const T & value
      ) noexcept
      : data() {
        std::fill(begin(),end(),value);
      }

      BAD(hd,inline)
      constexpr store_(
        std::initializer_list<T> list
      ) noexcept
      : data() {
        assert(list.size() <= D);
        std::copy(list.begin(),list.end(),begin());
      }

      template <class B>
      BAD(hd,inline)
      constexpr store_(store_expr<B,dim> const & rhs) noexcept {
        for (size_t i=0;i<D;++i)
          at(i) = rhs[i];
      }

      T data[size];

      BAD(reinitializes,hd,inline)
      store_ & operator =(T value) noexcept {
        for (size_t i=0;i<D;++i)
          at(i) = value;
        return *this;
      }

      BAD(hd,inline)
      store_ & operator =(std::initializer_list<T> list) noexcept {
        assert(list.size()<=D);
        std::copy(list.begin(),list.end(),begin());
        return *this;
      }

      BAD(hd,inline,const)
      plane & at(size_t i) noexcept {
        return reinterpret_cast<plane &>(data[delta_offset + i*S]);
      }

      BAD(hd,inline,const)
      plane const & at(size_t i) const noexcept {
        return reinterpret_cast<plane const &>(data[delta_offset + i*S]);
      }

      BAD(hd,inline,const)
      plane & operator[](size_t i) noexcept {
        return reinterpret_cast<plane &>(data[delta_offset + i*S]);
      }

      BAD(hd,inline,const)
      plane const & operator[](size_t i) const noexcept {
        return reinterpret_cast<plane const &>(data[delta_offset + i*S]);
      }

      BAD(hd,inline,const)
      store_ const & operator()() const noexcept { return *this; }

      template <class... Args>
      BAD(hd,inline,flatten)
      auto const &  operator ()(size_t i, Args... args) const noexcept {
        return (*this)[i](args...);
      }

      BAD(hd,inline,const)
      store_ & operator()() noexcept { return *this; }

      template <class... Args>
      BAD(hd,inline,flatten)
      auto & operator ()(size_t i, Args... args) noexcept {
        return (*this)[i](args...);
      }

      template <class B>
      BAD(reinitializes,hd,inline,flatten)
      store_ & operator = (store_expr<B,dim> const & rhs) noexcept {
        for (size_t i=0;i<D;++i)
          at(i) = rhs[i];
        return *this;
      }

      template <class B>
      BAD(hd,inline,flatten)
      store_ & operator += (store_expr<B,dim> const & rhs) noexcept {
        for (size_t i=0;i<D;++i)
          at(i) += rhs[i];
        return *this;
      }

      template <class B>
      BAD(hd,inline,flatten)
      store_ & operator -= (store_expr<B,dim> const & rhs) noexcept {
        for (size_t i=0;i<D;++i)
          at(i) -= rhs[i];
        return *this;
      }

      template <class B>
      BAD(hd,inline,flatten)
      store_ & operator *= (store_expr<B,dim> const & rhs) noexcept {
        for (size_t i=0;i<D;++i)
          at(i) *= rhs[i];
        return *this;
      }

      template <size_t N>
      using store_pull = store_<T, seq_pull<N,dim>, seq_pull<N,stride>>;

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

      template <size_t R>
      BAD(hd,inline,const)
      auto & rep() const noexcept {
        return reinterpret_cast<store_<T,seq_cons<R,dim>,seq_cons<ssize_t(0),stride>> const &>(*this);
      }

      template <size_t R>
      BAD(hd,inline,const)
      auto & rep() noexcept {
        return reinterpret_cast<store_<T,seq_cons<R,dim>,seq_cons<ssize_t(0),stride>> &>(*this);
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
        const_iterator & operator +=(ptrdiff_t d) const noexcept {
          i += d;
          return *this;
        }

        BAD(hd,inline,noalias)
        const_iterator & operator -=(ptrdiff_t d) const noexcept {
          i -= d;
          return *this;
        }

        BAD(hd,inline,pure)
        reference operator *() const noexcept {
          return *reinterpret_cast<pointer>(p + i*S);
        }

        BAD(hd,inline,pure)
        pointer operator ->() const noexcept {
          return reinterpret_cast<pointer>(p + i*S);
        }

        BAD(hd,inline,pure)
        reference operator[](difference_type di) const noexcept {
          return *reinterpret_cast<pointer>(p + (i+di)*S);
        }

        BAD(hd,inline,pure)
        bool valid() const noexcept {
          return p != nullptr && 0 <= i && i <= D;
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
          return { p, i };
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
          return { p, i++ };
        }

        BAD(hd,inline,noalias)
        iterator & operator --() noexcept {
          --i;
          return *this;
        }

        BAD(hd,inline,noalias)
        iterator operator --(int) noexcept {
          return { p, i-- };
        }

        BAD(hd,inline,pure)
        friend iterator operator +(iterator lhs, ptrdiff_t rhs) noexcept {
          return { lhs.p, lhs.i + rhs };
        }

        BAD(hd,inline,pure)
        friend iterator operator -(iterator lhs, ptrdiff_t rhs) noexcept {
          return { lhs.p, lhs.i - rhs };
        }

        BAD(hd,inline,pure)
        friend ptrdiff_t operator -(iterator lhs, iterator rhs) noexcept {
          assert(lhs.p == rhs.p);
          return lhs.i - rhs.i;
        }

        BAD(hd,inline,noalias)
        iterator & operator +=(ptrdiff_t d) const noexcept {
          i += d;
          return *this;
        }

        BAD(hd,inline,noalias)
        iterator & operator -=(ptrdiff_t d) const noexcept {
          i -= d;
          return *this;
        }

        BAD(hd,inline,pure)
        reference operator *() const noexcept {
          return *reinterpret_cast<pointer>(p + i*S);
        }

        BAD(hd,inline,pure)
        pointer operator ->() const noexcept {
          return reinterpret_cast<pointer>(p + i*S);
        }

        BAD(hd,inline,pure)
        reference operator[](ptrdiff_t di) const noexcept {
          return *reinterpret_cast<pointer>(p + (i+di)*S);
        }

        BAD(hd,inline,pure)
        bool valid() const noexcept {
          return 0 <= i && i <= D;
        }

        friend iterator operator + (ptrdiff_t lhs, iterator & rhs) {
          return { rhs.p, rhs.i + lhs };
        }
      };

      using reverse_iterator = std::reverse_iterator<iterator>;
      using const_reverse_iterator = std::reverse_iterator<const_iterator>;

      BAD(hd,inline,const)
      iterator begin() noexcept {
        return iterator(data + offset, 0);
      }

      BAD(hd,inline,const)
      iterator end() noexcept {
        return iterator(data + offset, D);
      }

      BAD(hd,inline,const)
      reverse_iterator rbegin() noexcept {
        return reverse_iterator(iterator(data + offset, D-1));
      }

      BAD(hd,inline,const)
      reverse_iterator rend() noexcept {
        return reverse_iterator(iterator(data + offset, -1));
      }

      BAD(hd,inline,const)
      const_iterator begin() const noexcept {
        return const_iterator(data + offset, 0);
      }

      BAD(hd,inline,const)
      const_iterator end() const noexcept {
        return const_iterator(data + offset, D);
      }

      BAD(hd,inline,const)
      const_iterator cbegin() const noexcept {
        return const_iterator(data + offset, 0);
      }

      BAD(hd,inline,const)
      const_iterator cend() const noexcept {
        return const_iterator(data + offset, D);
      }

      BAD(hd,inline,const)
      const_reverse_iterator rbegin() const noexcept {
        return reverse_iterator(const_iterator(data + offset, D-1));
      }

      BAD(hd,inline,const)
      const_reverse_iterator rend() const noexcept {
        return reverse_iterator(const_iterator(data + offset, -1));
      }

      BAD(hd,inline,const)
      const_reverse_iterator crbegin() const noexcept {
        return reverse_iterator(const_iterator(data + offset, D-1));
      }

      BAD(hd,inline,const)
      const_reverse_iterator crend() const noexcept {
        return reverse_iterator(const_iterator(data + offset, -1));
      }
    };
  }

  namespace detail {
    template <typename T, typename Dim, typename Stride1, typename Stride2>
    BAD(hd,inline,flatten)
    void swap(
      BAD(noescape) store_<T,Dim,Stride1> & l,
      BAD(noescape) store_<T,Dim,Stride2> & r
    ) noexcept {
      using std::swap;
      for (size_t i=0;i<l.D;++i)
        swap(l.at(i),r.at(i));
    }
  }

  using detail::swap;

  namespace detail {
    template<class T, class Dim, class Stride>
    BAD(hd)
    std::ostream &operator <<(std::ostream &os, const store_<T,Dim,Stride> & rhs) {
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
