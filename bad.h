#pragma once
#include <tuple>
#include <cstdint>
#include <limits>
#include <vector>
#include <type_traits>

// debug
#include <dlfcn.h>
#include <iostream>
#include "type.h"
#include <cstddef>
namespace autodiff {
  namespace detail {
    template <typename T>
    inline T * offset(T *ptr, std::ptrdiff_t delta) {
      return reinterpret_cast<T*>(reinterpret_cast<std::byte *>(ptr) + delta);
    }

    template <typename T, typename Act = T*> struct record;

    // a segment represents a slab of memory that is allocated from internally. the current pointer
    // is constantly decreased to make room until we run out of space.
    // the last record in each segment is either a link or a terminator
    // and owns the next segment
    //
    // a tape is a holder for the current segment as well as a cumulative total of 'activation records'
    // which tells us how much memory to allocate to propagate activation during the backwards sweep
    // of our automatic differentiation pass.
    //
    // TODO: minimum alignment
    // TODO: allow for cuda memory allocation
    // TODO: __host__ __device__ markers
    template <typename T, typename Act = T*> struct segment {
      using record_t = record<T,Act>;

      static constexpr size_t minimum_size = 65536;

      record_t * current;
      std::byte * memory;

      segment(const segment &) = delete;
      segment & operator=(segment const&) = delete;

    private:
      segment(std::byte * memory, size_t size) noexcept
      : current(reinterpret_cast<record<T>*>(memory + size))
      , memory(memory) {
      }

    public:
      segment() noexcept : current(nullptr), memory(nullptr) {};
      segment(size_t n) noexcept;
      segment(size_t n, segment && next) noexcept;
      segment(record_t * current, std::byte * memory) : current(current), memory(memory) {}
      segment(segment && rhs) noexcept
      : current(std::move(rhs.current))
      , memory(std::move(rhs.memory)) {
        rhs.current = nullptr;
        rhs.memory = nullptr;
      }

      ~segment() noexcept;

      segment & operator=(segment && rhs) noexcept;
    };


    template <typename T, typename Act>
    void swap(segment<T, Act> & a, segment<T, Act> & b) {
      using std::swap;
      swap(a.current, b.current);
      swap(a.memory, b.memory);
    }
  } // detail

  // forward declaration of tape
  template <typename T, typename Act = T*> struct tape;

  namespace detail {

    // implement record
    template <typename T, typename Act>
    struct record {
      using tape_t = tape<T,Act>;
      using segment_t = segment<T, Act>;

      record() noexcept {}
      // disable copy construction
      record(const record &) = delete;
      record & operator=(const record &) = delete;

      virtual record * next() noexcept = 0;
      virtual record const * next() const noexcept = 0;
      virtual ~record() noexcept {}
      virtual std::ostream & what(std::ostream & os) const noexcept = 0;

      // unlike usual, the result can be reached through the tape.
      [[maybe_unused]] void * operator new(size_t size, tape_t & tape) noexcept;

      // used internally. returns null if the segment is out of room.
      [[maybe_unused]] void * operator new(size_t size, segment_t & segment) noexcept;

      // we don't use the argument
      void operator delete([[maybe_unused]] void * data) noexcept {}

      // disable other new/delete forms:
      void * operator new  (size_t) = delete;
      void * operator new  (size_t, void *) noexcept = delete;
      void * operator new  (size_t, const std::nothrow_t &) = delete;
      void * operator new  (size_t, const std::align_val_t &, const std::nothrow_t &) = delete;
      void * operator new[](size_t) = delete;
      void * operator new[] (size_t, void *) noexcept = delete;
      void * operator new[](size_t, const std::nothrow_t &) = delete;
      void * operator new[](size_t, const std::align_val_t &, const std::nothrow_t &) = delete;
      void operator delete[](void *) noexcept = delete;
      void operator delete[](void *, size_t) noexcept = delete;
      void operator delete[](void *, std::align_val_t) noexcept = delete;
      void operator delete[](void *, size_t, std::align_val_t) noexcept = delete;

      // now we have to add a bunch of stuff for doing propagation
      virtual size_t activation_records() const noexcept { return 0; }
    };

    template <typename T, typename Act>
    std::ostream & operator << (std::ostream & os, const record<T, Act> & d) {
      return d.what(os);
    }

    template <typename T, typename Act>
    void * record<T,Act>::operator new(size_t t, segment_t & segment) noexcept {
      if (segment.memory == nullptr) return nullptr;
      std::byte * p = reinterpret_cast<std::byte *>(segment.current);
      if (p - segment.memory < t) return nullptr;
      p -= t;
      segment.current = reinterpret_cast<record<T, Act>*>(p);
      return static_cast<void *>(p);
    }

    // details that needed record to be filled in
    template <typename T, typename Act>
    segment<T, Act>::~segment() noexcept {
      if (current != nullptr) current->~record();
      if (memory != nullptr) delete[] memory;
      current = nullptr;
      memory = nullptr;
    }

    template <typename T, typename Act>
    segment<T, Act> & segment<T, Act>::operator=(segment<T, Act> && rhs) noexcept {
      using std::swap;
      swap(*this,rhs);
      return *this;
    }

    template <typename T, typename Act = T*>
    struct terminator : record<T, Act> {
      record<T,Act> * next() noexcept override { return nullptr; }
      record<T,Act> const * next() const noexcept override { return nullptr; }
      std::ostream & what(std::ostream & os) const noexcept override { return os << "terminator"; }
    };

    template <typename T, typename Act> segment<T, Act>::segment(size_t n) noexcept : segment(new std::byte[n], n) {
      new(*this) terminator<T,Act>();
    }

    template <typename T, typename Act = T*>
    struct link: record<T, Act> {
      link() = delete;
      link(segment<T,Act> && segment) noexcept : segment(std::move(segment)) {}
      ~link() noexcept override {}

      record<T,Act> * next() noexcept override { return segment.current; }
      record<T,Act> const * next() const noexcept override { return segment.current; }
      std::ostream & what(std::ostream & os) const noexcept override { return os << "link"; }
      segment<T,Act> segment;
    };

    template <typename T, typename Act>
    segment<T, Act>::segment(size_t n, segment<T, Act> && next) noexcept : segment(n) {
      if (next.memory != nullptr) {
        new(*this) link(std::move(next));
      } else {
        new(*this) terminator<T,Act>();
      }
    }

    template <typename A>
    struct intrusive_iterator : std::iterator<std::forward_iterator_tag, A> {
      using pointer = A*;
      using reference = A&;
      using const_pointer = std::add_const_t<pointer>;
      using const_reference = std::add_const_t<reference>;

      pointer p;

      intrusive_iterator() : p() {}
      intrusive_iterator(pointer p) noexcept : p(p) {}
      intrusive_iterator(const intrusive_iterator & rhs) noexcept : p(rhs.p) {}
      intrusive_iterator(intrusive_iterator &&  rhs) noexcept : p(std::move(rhs.p)) {}

      ~intrusive_iterator() noexcept {}

      constexpr bool operator == (const intrusive_iterator & rhs) const noexcept { return p == rhs.p; }
      constexpr bool operator != (const intrusive_iterator & rhs) const noexcept { return p != rhs.p; }

      constexpr reference operator *() const { return *p; }
      constexpr pointer operator -> () { return p; }
      intrusive_iterator & operator ++ () noexcept {
        assert(p != nullptr);
        p = p->next();
        return *this;
      }

      intrusive_iterator operator ++ (int) noexcept {
        assert(p != nullptr);
        auto q = p;
        p = p->next();
        return q;
      }

      constexpr pointer ptr() noexcept { return p; }
      constexpr const_pointer const_ptr() const noexcept { return p; }
      constexpr operator bool() const noexcept { return p != nullptr; }

      // template <typename = std::enable_if_v<!std::is_const_v(A)> >
      constexpr operator intrusive_iterator<const A> () const noexcept { return p; }
    };

    template <typename A>
    void swap (intrusive_iterator<A> & a, intrusive_iterator<A> & b) {
      using std::swap;
      swap(a.p,b.p);
    }
  } // detail

  // the workhorse
  template <typename T, typename Act>
  struct tape {
  protected:
    using segment_t = detail::segment<T,Act>;
    using record_t = detail::record<T, Act>;
  public:

    segment_t segment; // current segment
    size_t activations;

    using iterator = detail::intrusive_iterator<record_t>;
    using const_iterator = detail::intrusive_iterator<const record_t>;

    tape() noexcept : segment(), activations() {}
    tape(tape && rhs) noexcept : segment(std::move(rhs.segment)), activations(std::move(rhs.activations)) {}

    tape(const tape &) = delete;

    [[maybe_unused]] tape & operator=(const tape &) = delete;
    [[maybe_unused]] tape & operator=(tape && rhs) noexcept {
      segment = std::move(rhs.segment);
      activations = rhs.activations;
      return *this;
    }

    // put more stuff in here
    template <typename U, typename ... Args>
    U & push(Args ... args) noexcept {
      static_assert(std::is_base_of_v<record_t, U>, "tape record not derived from record<T>");
      auto result = new (*this) U(std::forward<args>...);
      activations += result->activation_records();
      return *result;
    }

    iterator begin() { return segment.current; }
    constexpr iterator end() { return iterator(); }

    const_iterator cbegin() const { return segment.current; }
    constexpr const_iterator cend() { return const_iterator(); }
  };

  template <typename T, typename Act>
  void swap(tape<T, Act> & a, tape<T, Act> & b) {
    using std::swap;
    swap(a.segment, b.segment);
    swap(a.activations, b.activations);
  }

  namespace detail {
    template <typename T, typename Act>
    void * record<T,Act>::operator new(size_t size, tape<T, Act> & tape) noexcept {
      auto result = record<T,Act>::operator new(size, tape.segment);
      if (result) return result;
      tape.segment = segment<T, Act>(std::max(segment_t::minimum_size, std::max(sizeof(link<T, Act>), sizeof(terminator<T, Act>)) + size), std::move(tape.segment));
      return record<T,Act>::operator new(size, tape.segment);
    }

    // a non-terminal entry designed for allocation in a slab
    template <typename B, typename T, typename Act = T &>
    struct propagator : record<T,Act> {
      propagator() : record<T,Act>() {

      }
      record<T, Act> const * next() const noexcept override {
        return reinterpret_cast<record<T, Act> const *>(reinterpret_cast<std::byte const*>(this) + sizeof(B));
      }
      record<T, Act> * next() noexcept override {
        return reinterpret_cast<record<T, Act> *>(reinterpret_cast<std::byte*>(this) + sizeof(B));
      }
      std::ostream & what(std::ostream & os) const noexcept override {
        return os << type(*static_cast<B const *>(this));
      }
    };

    // a non-terminal entry designed for allocation in a slab, that produce a fixed number of activation records
    template <size_t Acts, typename B, typename T, typename Act = T*>
    struct static_propagator : propagator<B,T,Act> {
      static_propagator() : propagator<B,T,Act>() {}
      static constexpr size_t acts = Acts;
      constexpr size_t activation_records() const noexcept override {
        return acts;
      }
    };
  } // namespace detail

  using std::swap;
  // using detail::swap;
} // namespace autodiff

/*

// END OF REBUILD

namespace autodiff {
  template <typename T, typename Act> struct tape;
  using index_t = std::uint_fast32_t;

  namespace detail {
    template <typename T, typename Act> struct ad_types {
      using element_t = T;
      using activation_t = Act;
      using tape_t = tape<T,Act>;
    };

    template <typename T, typename Act>
    using node = void (*)(T const * & e, index_t const * & i, Act act, index_t & base);
  };

  // default activations are for single results
  template <typename T, typename Act = T*>
  struct tape : detail::ad_types<T,Act> {
    std::vector<T> data;
    std::vector<index_t> indices;
    std::vector<detail::node<T,Act>> entries;
    index_t head; // scalar activation records required

    tape(int head = 0) : data(), indices(), entries(), head(head) {}

    bool operator==(const tape &rhs) const { return this = &rhs; }

    void propagate(Act a) {
      index_t b = head;
      T const * d = data.data()+data.size(); // .cend();
      index_t const * i = indices.data() + indices.size(); // indices.cend();
      for (auto e = entries.rbegin(); e != entries.rend(); ++e)
        (*e)(d,i,a,b);
    }

    void propagate_verbose(Act a) {
      using namespace std;
      index_t b = head;
      T const * d = data.data()+data.size(); // .cend();
      index_t const * i = indices.data() + indices.size(); // indices.cend();
      Dl_info info;
      for (auto e = entries.rbegin(); e != entries.rend(); ++e) {
        bool ok = dladdr(reinterpret_cast<void *>(*e),&info) != 0;
        assert(ok);
        T const * od = d;
        index_t const * oi = i;
        index_t ob = b;
        cout << demangle(info.dli_sname) << endl;
        (*e)(d,i,a,b);
        cout << (ob-b) << " activation" << (ob-b == 1 ? "" : "s") << " starting at " << b << endl;
        if (d != od) { cout << "data: "; copy(d, od, ostream_iterator<T>(cout, " ")); cout << endl; }
        if (i != oi) { cout << "indices: "; copy(i, oi, ostream_iterator<T>(cout, " ")); cout << endl; }
      }
    }

    index_t advance(int i = 1) {
      //std::cout << "advancing by " << i << " from " << head << std::endl;
      auto old = head;
      head += i;
      return old;
    }
  };

  namespace detail {
    template <typename T, typename Act, T ... Ks>
    void static_linear(T const * & e, index_t const * & i, Act a, index_t & b) {
      using namespace std;

      index_t lb = --b;
      constexpr index_t di = sizeof...(Ks);
      index_t const * j = i -= di;
      constexpr T ts[di] = { Ks ... };
      for (const T & t : ts) {
        a[*j++] += t * a[lb];
      }
    }

    template <typename T, typename Act, int N>
    void dynamic_linear(T const * & e, index_t const * & i, Act a, index_t & b) {
      index_t lb = --b;
      index_t const * j = i -= N;
      const T * f = e -= N;
      for (index_t k = 0;k<N;++k)
        a[j[k]] += f[k] * a[lb];
    }
  };

  template <typename T, index_t ... sizes>
  struct tensor : expr<T, tensor<T, sizes...>>  {
    constexpr size = (sizes * ...);

    template <typename ... Args>
    int index(Args ... args) {
      static_assert(sizeof...(args) == sizeof...(sizes));
      // smallest dimension to largest dimension, this is the reverse of my usual convention

      constexpr index_t dims[] = { sizes ... };
      // zip the dims backwards through the argument list



    }


    std::array<

  };

  constexpr index_t no_index = std::numeric_limits<index_t>::max();

  template <typename T, typename B>
  struct expr {
     T primal() const {
       return static_cast<B const &>(*this).primal();
     }

     template <typename Tape, typename Act>
     void propagate(const Tape & tape, Act act, index_t & index) const {
       static_cast<B const &>(*this).propagate(tape, act, index);
     }

     static constexpr std::size_t size() { return sizeof(B); }
  };

  template <typename T, typename L, typename R>
  struct expr_sum : expr<T, expr_sum<T,L,R>>{
    L const & lhs;
    R const & rhs;
    T primal() const {
      return lhs.primal() + rhs.primal();
    }
    template <typename Tape, typename Act>
    void propagate(const Tape & tape, Act act, index_t & index) const {
      lhs.propagate(tape, act, index_t);
      rhs.propagate(tape, act, index_t);
    }
  }

  // ad<T> -- uses the tape and scalar operations
  template <typename T, typename Act = T*>
  struct ad {

    using tape_t = tape<T, Act>;

    mutable tape_t * tape;
    index_t index;

    T primal;

    template <typename ... Args>
    void push_index(Args ... is) const noexcept {
      assert(tape != nullptr);
      (tape->indices.emplace_back(is),...);
    }

    template <typename ... Args>
    void push_entry(Args ... is) const noexcept {
      assert(tape != nullptr);
      (tape->entries.emplace_back(is),...);
    }

    template <typename ... Args>
    void push_data(Args ... is) const noexcept {
      assert(tape != nullptr);
      (tape->data.emplace_back(is),...);
    }

    index_t advance(int i = 1) const noexcept {
      assert(tape != nullptr);
      return tape->advance(i);
    }

    // lifting constants
    ad() noexcept // noexcept(noexcept(T()) && noexcept(TapeRef()))
    : primal()
    , index(no_index)
    , tape() {}

    ad(ad && rhs) noexcept // noexcept(noexcept(T(std::move_if_noexcept(rhs.primal))) && noexcept(TapeRef(std::move_if_noexcept(rhs.tape))))
    : primal(std::move_if_noexcept(rhs.primal))
    , index(std::move(rhs.index))
    , tape(std::move_if_noexcept(rhs.tape)) {}

    ad(const ad & rhs) noexcept // noexcept(noexcept(T(rhs.primal)) && noexcept(TapeRef(rhs.tape))
    : primal(rhs.primal)
    , index(rhs.index)
    , tape(rhs.tape) {}

    // lift
    ad(const T & primal) noexcept : primal(primal), tape(), index(no_index) {}

    // evil
    ad(const T & primal, tape_t * tape, index_t index) noexcept : primal(primal), tape(tape), index(index) {}

    bool is_const() const noexcept { return index == no_index; }

    ad operator * (const T & rhs) const noexcept {
      auto p = primal * rhs;
      if (is_const()) return ad(p);
      push_index(index);
      push_data(rhs);
      push_entry(&detail::dynamic_linear<T,Act,1>);
      return ad(p, tape, advance());
    }

    ad operator * (const ad & rhs) const noexcept {
      auto p = primal * rhs.primal;
      if (is_const()) {
        if (rhs.is_const()) return ad(p);
        // constant * non-constant
        rhs.push_index(rhs.index);
        rhs.push_data(primal);
        rhs.push_entry(&detail::dynamic_linear<T,Act,1>);
        return ad(p, rhs.tape, rhs.advance());
      }

      if (rhs.is_const()) {
        // non-constant * constant
        push_index(index);
        push_data(rhs.primal);
        push_entry(&detail::dynamic_linear<T,Act,1>);
        return ad(p, tape, advance());
      }

      // non-constant * non-constant
      assert(tape == rhs.tape);
      push_index(index, rhs.index);
      push_data(rhs.primal, primal);
      push_entry(&detail::dynamic_linear<T,Act,2>);
      return ad(p, tape, advance());
    }

    ad & operator *= (const T & rhs) const noexcept {
      primal *= rhs;
      if (!is_const()) {
        push_index(index);
        push_data(rhs.primal);
        push_entry(&detail::dynamic_linear<T,Act,1>);
        index = advance();
      }
      return *this;
    }

    ad operator + (const T & rhs) const noexcept { return ad(primal + rhs, tape, index); }
    ad operator - (const T & rhs) const noexcept { return ad(primal - rhs, tape, index); }

    ad & operator += (const T & rhs) noexcept { primal += rhs; return *this; }
    ad & operator -= (const T & rhs) noexcept { primal -= rhs; return *this; }

    ad operator + (const ad & rhs) const noexcept {
      auto p = primal + rhs.primal;
      if (is_const()) return ad(p, rhs.tape, rhs.index);
      if (rhs.is_const()) return ad(p, tape, index);
      // both sides non-constant
      push_index(index, rhs.index);
      push_entry(&detail::static_linear<T,Act,1,1>);
      index_t new_index = advance();
      std::cout << index << ", " << rhs.index << " -> " << new_index << std::endl;
      return ad(p, tape, new_index);
    }

    ad & operator -= (const ad & rhs) noexcept {
      primal -= rhs.primal;
      if (!rhs.is_const())  {
        if (is_const()) {
          tape = rhs.tape; // we had a bogus tape, copy it if needed.
          push_entry(&detail::static_linear<T,Act,-1>);
        } else {
          assert(tape == rhs.tape);
          push_entry(&detail::static_linear<T,Act,1,-1>);
          push_index(index);
        }
        push_index(rhs.index);
        index = advance();
      }
      return *this;
    }

    ad operator - (const ad & rhs) const noexcept {
      auto p = primal - rhs.primal;
      if (rhs.is_const()) return ad(p, tape, index);
      if (is_const()) {
        rhs.push_index(index);
        rhs.push_entry(&detail::static_linear<T,Act,-1>);
        return ad(p, rhs.tape, rhs.advance());
      }
      // non-constant - non-constant
      assert(tape == rhs.tape);
      push_index(index, rhs.index);
      push_entry(&detail::static_linear<T,Act,1,-1>);
      return ad(p, tape, advance());
    }
  }; // ad

  // left hand scalar multiplication
  template <typename T, typename Act> ad<T,Act> operator + (const T & lhs, const ad<T,Act> & rhs) {
    return ad(lhs + rhs.primal, rhs.tape, rhs.index);
  }

  template <typename T, typename Act> ad<T,Act> operator - (const T & lhs, const ad<T,Act> & rhs) {
    auto p = lhs - rhs.primal;
    if (rhs.is_const()) return p;
    rhs.push_index(rhs.index);
    rhs.push_entry(&detail::static_linear<T,Act,-1>);
    return ad(p, rhs.tape, rhs.advance());
  }

  template <typename T, typename Act> ad<T,Act> operator * (const T & lhs, const ad<T,Act> & rhs) {
    auto p = lhs * rhs.primal;
    if (rhs.is_const()) return p;
    rhs.push_index(rhs.index);
    rhs.push_data(lhs);
    rhs.push_entry(&detail::dynamic_linear<T,Act,1>);
    return ad(p, rhs.tape, rhs.advance());
  }


  template<typename T>
  std::ostream &operator <<(std::ostream &os, const std::vector<T> &v) {
    using namespace std;
    copy(v.begin(), v.end(), ostream_iterator<T>(os, "\n"));
    return os;
  }
}

template<typename T, typename Act>
std::ostream &operator <<(std::ostream &os, void (*z)(T const * & e, autodiff::index_t const * & i, Act a, autodiff::index_t & b)) {
  Dl_info info;
  if (dladdr(reinterpret_cast<const void *>(&z), &info)) {
    return os << info.dli_sname;
  } else {
    return os << "NO"; // reinterpret_cast<intptr_t>(z);
  }
}

namespace autodiff {
  template <typename T, typename F>
  std::tuple<T,T> diff(F f, T a) {
    tape<T> t(1); // one entry
    ad<T> result = f(ad<T>(a,&t,0));
    int N = t.head;
    std::unique_ptr<T[]> activations (new T[N]);
    for (int i=0;i<N;++i) activations[i] = 0;
    if (N != 0) activations[N-1] = 1;
    t.propagate_verbose(activations.get());
    return { result.primal, activations[0] };
  }
} // namespace autodiff

*/
