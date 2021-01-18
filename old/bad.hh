#pragma once
#include <utility>
#include "tape.hh"
#include "shape.hh"

namespace bad {

  // writeable view, acts like a pointer.
  template <typename T, typename Stride, typename Dim> struct lens{};

  // a lens is a pointer into a matrix, not a matrix
  template <typename T, index_t S, index_t D>
  struct lens<T, seq<S>, seq<D>> {
    using iterator       = cursor<T, seq<S>, seq<D>>;
    using const_iterator = const_cursor<const T, seq<S>, seq<D>>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    T * data;
    explicit lens(T * data) : data(data) {}
    lens(const lens & rhs) : data(rhs.data) {}
    lens(lens && rhs) : data(std::move(rhs.data)) {}
    T & operator[](index_t i) noexcept { return data[i*S]; }
    const T & operator[](index_t i) const noexcept { return data[i*S]; }
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
  };

  template <typename T, index_t S, index_t ... Ss, index_t D, index_t ... Ds>
  struct lens<T, seq<S,Ss...>, seq<D,Ds...>> {
    using iterator = cursor<T, seq<S,Ss...>, seq<D,Ds...>>;
    using const_iterator = const_cursor<T, seq<S,Ss...>, seq<D,Ds...>>;
     T * data;
     explicit lens(T * data) : data(data) {}
     lens<T, seq<Ss...>, seq<D,Ds...>> operator [](index_t i) noexcept {
       return lens(data + i*S);
     }
     const lens<T, seq<Ss...>, seq<D,Ds...>> operator [](index_t i) const noexcept {
       return lens(data + i*S);
     }
     // todo: iterator, const_iterator
  };

  template <diff_t i, typename T, ty
  pename S, index_t D, index_t ... Ds>
  auto get(lens<T, S, seq<D, Ds...> l) {
    static_assert(0 < i, "negative index")
    static_assert(i < D, "index too large");
    return l[i];
  }


  };
  // offer iteration services as well?
  template <typename T, size_t S, size_t D> struct lens {
     T * data;
     T & operator[](index_t i) noexcept {
       return data[i*S];
     }
     const T & operator[](index_t i) const noexcept {
       return data[i*S];
     }
  };
  template <typename T, size_t S, size_t ... Ss> struct lens {
     T * data;
     lens<T, Ss...> operator[](index i) noexcept {

     }

  };

  template <typename B, typename T, typename Act = T*, index_t ... Shape>
  struct expr {
    static constexpr index_t size = prod<Shape...>;
    // the dimensions used
    using shape_t = std::integer_sequence<index_t, Shape...>;
    // a sequence of 0,1,2,3,4 up to dimension.
    using seq_t = std::make_integer_sequence<int, sizeof...(Shape)>;

    template <int N> static constexpr index_t step = stride<N,index_t,Shape...>;
    template <int N> static constexpr index_t dim = nth<N,index_t,Shape...>;

  private:
    template <typename E> struct steps_;
    template <int ... Is> struct steps_<std::integer_sequence<index_t, Is...>> {
      static constexpr index_t value[] = { step<Is> ... };
    };
  public:
    static constexpr index_t steps[] = steps_<std::make_integer_sequence<int, sizeof...(Shape)>>::value;

    static constexpr bool is_simple = B::is_simple;
    // simple should let me know this is just a mapping operation

    // compute the primal value
    template <typename ... Args>
    inline T primal(Args ... args) const {
      static_assert(sizeof...(args) == sizeof...(Shape));
      return static_cast<B const &>(*this).primal(args...);
    }

    template <typename ... Args>
    inline void load(T * data, Args ... args) {
      constexpr index_t N = sizeof...(Args);
      if constexpr (N == sizeof...(Shape)) {
        *data = primal(args...);
      } else {
        constexpr int di = step<N>;
        for (index_t i=0; i<N; ++i, data += di)
          load(data, i, args ...);
      }
    }

    // only available of is_simple says yes
    template <typename = std::enable_if_t<is_simple> >
    void load_simple(T * data) {
      return static_cast<B const &>(*this).load_simple(data);
    }
  };

  template <typename T, typename Act = T*, index_t ... Shape>
  struct ad : expr<ad<T,Act,Shape...>,T,Act,Shape...> {
    using tape_t = tape<T, Act>;
    using shape_t = std::integer_sequence<index_t, Shape...>;
    mutable tape_t * tape;
    index_t index;
    T primals[prod<Shape...>];

    template <typename B>
    ad(expr<B, T, Act, Shape...> const & expr) : primals() {
      if constexpr (B::is_simple) {
        expr.load_simple(primals);
      } else {
        expr.load(primals);
      }
    }

    template <typename ... Args>
    inline T primal(Args ... args) const {
      // TODO:
      return T();
      //      return primals[flat<Shape...>(args...)];
    }
  };
}
