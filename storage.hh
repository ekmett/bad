#pragma once
#include <array>
#include "seq.hh"
#include "attrib.hh"

#pragma STDC FP_CONTRACT ON

namespace bad {
  template <class dim>
  struct storage;

  // default to row major tensor<int,1,2,3,4> -- is convenient to write
  template <class T, size_t d, size_t... ds>
  using tensor = typename storage<sseq<d,ds...>>::template type<T>;

  //template <class B, size_t d, size_t... ds>
  //using storage_expr = typename storage<sseq<d,ds...>>::template expr<B>;

  // store<int,seq<1,2,3,4>,sseq<24,12,4,1>> is less convenient but more powerful than tensor which picks row-major always
  template <class T, class dim, class stride = row_major<dim>>
  using store = typename storage<dim>::template type<T,stride>;

  // a 0-dimensional tensor is a scalar
  template <>
  struct storage<seq<>> {
    using dim = seq<>;

    template <class T,class = sseq<>>
    using type = T;
  };

  template <class B, size_t d,size_t... ds>
  struct storage_expr {
    using dim = seq<d,ds...>;
    static constexpr size_t arity = 1 + sizeof...(ds);

    template <size_t i>
    static constexpr size_t nth_dim = nth<i,d,ds...>;

    template <class C = B>
    using expr = storage_expr<C,d,ds...>;

    using actual_type = B;
    // using element = typename B::element;

    BAD(hd,inline,const)
    B const & at() const noexcept {
      return static_cast<B const &>(*this);
    }

    template <class... ts>
    BAD(hd,inline,flatten) // this lifetimebound
    auto at(size_t i, ts... is) const noexcept {
      return at()[i](is...);
    }

    BAD(hd,inline,flatten)
    B const & operator[](size_t i) const noexcept {
      return at(i);
    }

    BAD(hd,inline,const)
    B const & operator()() const noexcept {
      return at();
    }

    template <class... ts>
    BAD(hd,inline,flatten) // this lifetimebound
    auto operator()(ts... is) const noexcept {
      return at(is...);
    }

    template <class C>
    BAD(hd,inline) // constexpr?
    bool operator==(expr<C> & r) const noexcept {
      auto l = at();
      for (size_t i=0;i<d;++i) {
        if (l[i] != r[i]) return false;
      }
      return true;
    }

    template <class C>
    BAD(hd,inline) // constexpr
    bool operator!=(expr<C> const & r) noexcept {
      auto l = at();
      for (size_t i=0;i<d;++i) {
        if (l[i] != r[i]) return true;
      }
      return false;
    }

    BAD(hd) 
    friend std::ostream operator <<(std::ostream &os, storage_expr const & rhs) {
      return os << rhs.at();
    }

    struct const_iterator {
      B const * p;
      ptrdiff_t i;

      using iterator_category = std::random_access_iterator_tag;
      using value_type        = decltype(p->at(0));
      using difference_type   = ptrdiff_t;
      using reference_type    = std::add_lvalue_reference<value_type>;
      using pointer_type      = std::add_pointer<value_type>;

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
      auto & operator *() const noexcept {
        return p->at(i);
      }

      BAD(hd,inline,pure)
      auto * operator ->() const noexcept {
        return &(p->at(i));
      }

      BAD(hd,inline,pure)
      auto & operator[](difference_type di) const noexcept {
        return p->at(i + di);
      }

      BAD(hd,inline,pure)
      bool valid() const noexcept {
        return p != nullptr && 0 <= i && i < d;
      }
    };

    struct iterator {
      B * p;
      ptrdiff_t i;

      using iterator_category = std::random_access_iterator_tag;
      using value_type        = std::remove_reference<decltype(p->at(0))>;
      using difference_type   = ptrdiff_t;
      using reference         = std::add_lvalue_reference<value_type>;
      using pointer           = std::add_pointer<value_type>;

      BAD(hd,inline,noalias) constexpr
      iterator(const iterator & rhs) noexcept
      : p(rhs.p), i(rhs.i) {}

      BAD(hd,inline,noalias) constexpr
      iterator(iterator && rhs) noexcept
      : p(std::move(rhs.p)), i(std::move(rhs.i)) {}

      BAD(reinitializes,hd,inline,noalias)
      iterator & operator=(iterator rhs) noexcept {
        p = rhs.p;
        i = rhs.i;
        return *this;
      }

      BAD(reinitializes,hd,inline,noalias)
      iterator & operator=(iterator && rhs) noexcept {
        p = std::move(rhs.p);
        i = std::move(rhs.i);
        return *this;
      }

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
        return p->at(i);
      }

      BAD(hd,inline,pure)
      pointer operator ->() const noexcept {
        return *(p->at(i));
      }

      BAD(hd,inline,pure)
      reference operator[](ptrdiff_t di) const noexcept {
        return p->at(i + di);
      }

      BAD(hd,inline,pure)
      bool valid() const noexcept {
        return 0 <= i && i < d;
      }

      friend iterator operator + (ptrdiff_t lhs, iterator & rhs) {
        return { rhs.p, rhs.i + lhs };
      }
    };

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    BAD(hd,inline,const)
    iterator begin() noexcept {
      return iterator(&at(), 0);
    }

    BAD(hd,inline,const)
    iterator end() noexcept {
      return iterator(&at(), d);
    }

    BAD(hd,inline,const)
    reverse_iterator rbegin() noexcept {
      return reverse_iterator(iterator(&at(), d-1));
    }

    BAD(hd,inline,const)
    reverse_iterator rend() noexcept {
      return reverse_iterator(iterator(&at(), -1));
    }

    BAD(hd,inline,const)
    const_iterator begin() const noexcept {
      return const_iterator(&at(), 0);
    }

    BAD(hd,inline,const)
    const_iterator end() const noexcept {
      return const_iterator(&at(), d);
    }

    BAD(hd,inline,const)
    const_iterator cbegin() const noexcept {
      return const_iterator(&at(), 0);
    }

    BAD(hd,inline,const)
    const_iterator cend() const noexcept {
      return const_iterator(&at(), d);
    }

    BAD(hd,inline,const)
    const_reverse_iterator rbegin() const noexcept {
      return reverse_iterator(const_iterator(&at(), d-1));
    }

    BAD(hd,inline,const)
    const_reverse_iterator rend() const noexcept {
      return reverse_iterator(const_iterator(&at(), -1));
    }

    BAD(hd,inline,const)
    const_reverse_iterator crbegin() const noexcept {
      return reverse_iterator(const_iterator(&at(), d-1));
    }

    BAD(hd,inline,const)
    const_reverse_iterator crend() const noexcept {
      return reverse_iterator(const_iterator(&at(), -1));
    }

  };

  template <size_t d,size_t... ds>
  struct storage<seq<d,ds...>> {
    using dim = seq<d,ds...>;
    static constexpr size_t arity = 1 + sizeof...(ds);
    template <size_t i>
    static constexpr size_t nth_dim = nth<i,d,ds...>;

    template <class B>
    using expr = storage_expr<B,d,ds...>;

    template <class B>
    struct rep_expr : expr<B> {
      using element = typename B::element;

      B const & base;

      BAD(hd,inline,pure)
      auto operator[](BAD(maybe_unused) size_t i) const noexcept {
        return base;
      }

      template <size_t N>
      BAD(hd,inline,flatten)
      auto pull(size_t i) const noexcept {
        if constexpr(N == 0) {
          return base;
        } else {
          return base.template pull<N-1>(i).template rep<d>();
        }
      }

      // this is lifetimebound, move out of line?
      template <size_t N>
      BAD(hd,inline,flatten,const)
      auto rep() const noexcept { 
        using reprep = typename storage<seq<N,d,ds...>>::template rep_expr<rep_expr>;
        return reprep { *this };
      }

      BAD(hd)
      friend std::ostream & operator<<(std::ostream & os, rep_expr const & rhs) {
        return os << "rep<" << d << ">(" << rhs << ")";
      }
    };

    template <class L, class R>
    struct add_expr {
      L const & l;
      R const & r;

      BAD(hd,inline,pure)
      auto operator[](BAD(maybe_unused) size_t i) const noexcept {
        return l[i] + r[i];
      }

      template <size_t N>
      BAD(hd,inline,flatten)
      auto pull(size_t i) const noexcept {
        return l.template pull<N>(i) + r.template pull<N>(i);
      }
      
      template <size_t N>
      BAD(hd,inline,flatten,const)
      auto rep() const noexcept { 
        using repadd = typename storage<seq<N,d,ds...>>::template rep_expr<rep_expr>;
        return repadd { *this };
        // return l.template rep<N>(i) + r.template rep<N>(i);
      }
    };

    // using a dependent partial template specialization because
    // default arguments cannot be supplied for parameter packs
    template <class T, class stride = row_major<dim>>
    struct type;

    template <class T, ptrdiff_t s, ptrdiff_t... ss>
    struct type<T, sseq<s,ss...>>
    : expr<type<T, sseq<s,ss...>>> {
      using element = T;

      using stride = sseq<s,ss...>;

      template <size_t i>
      static constexpr ptrdiff_t nth_stride = nth<i,s,ss...>;

      using plane = store<T,seq<ds...>,sseq<ss...>>;

    private:
      template <size_t i>
      static constexpr ptrdiff_t nth_extremum = nth_stride<i>*(nth_dim<i>-1);

      using super = expr<type<T,sseq<s,ss...>>>;

      template <class>
      struct calc_t;

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
      constexpr type() noexcept : data() {
      }

      BAD(hd,inline)
      constexpr type(
        T value
      ) noexcept
      : data() {
        std::fill(begin(),end(),value);
      }

      BAD(hd,inline)
      constexpr type(
        std::initializer_list<T> list
      ) noexcept
      : data() {
        assert(list.size() <= d);
        std::copy(list.begin(),list.end(),begin());
      }

      template <class B>
      BAD(hd,inline)
      constexpr type(expr<B> const & rhs) noexcept {
        for (size_t i=0;i<d;++i)
          at(i) = rhs[i];
      }

      // this should lifetimebound
      BAD(hd,inline,const)
      auto operator[](size_t i) noexcept -> plane & {
        return reinterpret_cast<plane &>(data[delta + i*s]);
      }

      BAD(hd,inline,const)
      auto operator[](size_t i) const noexcept -> plane const & {
        return reinterpret_cast<plane const &>(data[delta + i*s]);
      }

      // tools that use []
      using super::at;
      using super::operator();

      template <class B>
      BAD(reinitializes,hd,inline,flatten)
      type & operator = (expr<B> const & rhs) noexcept {
        for (size_t i=0;i<d;++i)
          at(i) = rhs[i];
        return *this;
      }

      template <class B>
      BAD(hd,inline,flatten)
      type & operator += (expr<B> const & rhs) noexcept {
        for (size_t i=0;i<d;++i)
          at(i) += rhs[i];
        return *this;
      }

      template <class B>
      BAD(hd,inline,flatten)
      type & operator -= (expr<B> const & rhs) noexcept {
        for (size_t i=0;i<d;++i)
          at(i) -= rhs[i];
        return *this;
      }

      template <class B>
      BAD(hd,inline,flatten)
      type & operator *= (expr<B> const & rhs) noexcept {
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
          return { rhs.p, rhs.i + lhs };
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
      friend std::ostream & operator<<(std::ostream &os, type const & rhs) {
        os << "{";
        for (size_t i=0;i<d;++i) {
          if (i) os << ",";
          os << rhs.data[i];
        }
        return os << "}";
      }
    }; // type

    // CTAD deduction guides
    template <class T, class stride> type(const type<T,stride> &) -> type<T,stride>;
    template <class B> type(const expr<B> &) -> type<typename B::element>; // implicitly row_major
    template <class T> type(T) -> type<T>; // implicitly row_major
  }; // storage

  template <size_t d0, class B, size_t d1, size_t... ds>
  BAD(hd,inline)
  auto rep(BAD(lifetimebound) storage_expr<B,d1,ds...> const & x) noexcept {
    return x().template rep<d0>();
  }

  template <size_t d0, class B, size_t d1, size_t... ds>
  BAD(hd,inline)
  auto rep(BAD(lifetimebound) storage_expr<B,d1,ds...> & x) noexcept {
    return x().template rep<d0>();
  }

  /// replicate base data types
  template <size_t d, class T>
  BAD(hd,inline)
  auto rep(T t) noexcept -> store<T,seq<d>,sseq<0>> {
    return t;
  }

  // * pull the nth dimension of a storage expression to the front.

  template <size_t N, class B, size_t d, size_t... ds>
  BAD(hd,inline)
  auto pull(
    BAD(lifetimebound) storage_expr<B,d,ds...> const & rhs,
    size_t i
  ) noexcept {
    return rhs().template pull<N>(i);
  }

  // * pull the nth dimension of a store to the front.

  template <size_t N, class B, size_t d, size_t... ds>
  BAD(hd,inline)
  auto pull(
    BAD(lifetimebound) storage_expr<B,d,ds...> & rhs,
    size_t i
  ) noexcept {
    return rhs().template pull<N>(i);
  }

  template <typename T, typename stride1, typename stride2, size_t d, size_t... ds>
  BAD(hd,inline,flatten)
  void swap(
    BAD(noescape) store<T,sseq<d,ds...>,stride1> & l,
    BAD(noescape) store<T,sseq<d,ds...>,stride2> & r
  ) noexcept {
    using std::swap;
    for (size_t i=0;i<d;++i)
      swap(l[i],r[i]);
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

/*
  // CTAD
  template <size_t d, size_t... ds, class B>
  show_values(typename storage<d,ds...>::template expr<B> const & data)
    -> show_values<d, typename storage<d,ds...>::template expr<B>>;
*/

  // CTAD
  template <size_t d, class T>
  show_values(T(&)[d])
    -> show_values<d,T*>;

  template <class L, class R, size_t d, size_t... ds>
  BAD(hd,inline,const)
  auto operator +(
    BAD(lifetimebound) storage_expr<L,d,ds...> const &l,
    BAD(lifetimebound) storage_expr<R,d,ds...> const &r
  ) noexcept -> typename storage<seq<d,ds...>>::template add_expr<L,R> {
    return { l() ,r() };
  }
}
