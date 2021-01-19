#pragma once
#include <tuple>
#include <cstdint>
#include <limits>
#include <vector>
#include <type_traits>
#include <cstdlib>
#include <dlfcn.h>
#include <cstddef>
#include <iostream>
#include "abi.hh"
#include "seq.hh"

namespace bad {
  using index_t = std::size_t;
  static constexpr index_t no_index = static_cast<index_t>(-1);
  static constexpr index_t record_alignment = 16;
  static constexpr index_t record_mask = static_cast<index_t>(~0xf);

  namespace detail {

    template <typename T>
    inline T * offset(T *ptr, std::ptrdiff_t delta) {
      return reinterpret_cast<T*>(reinterpret_cast<std::byte *>(ptr) + delta);
    }

    inline bool is_aligned(const void * ptr, std::uintptr_t alignment) noexcept {
      auto iptr = reinterpret_cast<std::uintptr_t>(ptr);
      return !(iptr % alignment);
    }

    template <typename T, typename Act = T*>
    struct record;

    // TODO: alignment
    // TODO: cuda memory allocation
    // TODO: __host__ __device__ markers
    template <typename T, typename Act = T*>
    struct segment {
      using record_t = record<T,Act>;

      static constexpr index_t minimum_size = 65536;

      record_t * current;
      std::byte * memory;

      segment(const segment &) = delete;
      segment & operator=(segment const&) = delete;

    private:
      segment(std::byte * memory, index_t size) noexcept
      : current(reinterpret_cast<record<T>*>(memory + size))
      , memory(memory) {
      }

    public:
      segment() noexcept : current(nullptr), memory(nullptr) {};
      segment(index_t n) noexcept;
      segment(index_t n, segment && next) noexcept;
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
  template <typename T, typename Act = T*>
  struct tape;

  namespace detail {
    static constexpr index_t pad_to_alignment(index_t i) {
      return (i + record_alignment - 1) & record_mask;
    }

    template <typename T, typename Act = T*> struct link;

    // implement record
    template <typename T, typename Act>
    struct alignas(record_alignment) record {
      using tape_t = tape<T,Act>;
      using segment_t = segment<T, Act>;
      using act_t = Act;
      using record_t = record<T, Act>;

      record() noexcept {}
      // disable copy construction
      record(const record &) = delete;
      record & operator=(const record &) = delete;

      virtual record * next() noexcept = 0;
      virtual record const * next() const noexcept = 0;
      virtual ~record() noexcept {
      }
      virtual std::ostream & what(std::ostream & os) const noexcept = 0;
      // now we have to add a bunch of stuff for doing propagation
      virtual index_t activation_records() const noexcept { return 0; }
      // should return the same answer as next
      virtual record const * propagate(Act act, index_t & i) const noexcept = 0;
      virtual link<T,Act> * as_link() noexcept { return nullptr; }

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
    };

    template <typename T, typename Act>
    std::ostream & operator << (std::ostream & os, const record<T, Act> & d) {
      return d.what(os);
    }

    template <typename T, typename Act>
    void * record<T,Act>::operator new(size_t t, segment_t & segment) noexcept {
      if (segment.memory == nullptr) return nullptr;
      std::byte * p = reinterpret_cast<std::byte *>(segment.current);
      t = pad_to_alignment(t);
      if (p - segment.memory < t) return nullptr;
      p -= t;
      segment.current = reinterpret_cast<record_t*>(p);
      // requires c++20
      //return std::assume_aligned<record_alignment>(static_cast<void *>(p));
      return static_cast<void *>(p);
    }

    template <typename T, typename Act>
    segment<T, Act>::~segment() noexcept {
      if (current != nullptr) {
        record<T, Act> * p = current;
        // this avoids building up a stack frame for each segment, but yeesh.
        while (p != nullptr) {
          record<T, Act> * np = p->next();
          link<T, Act> * link = p->as_link();
          if (link) {
            // we're going to become it
            segment<T, Act> temp = std::move(link->segment);
            p->~record();
            std::free(memory);
            memory = nullptr;
            current = nullptr;
            swap(*this,temp);
          } else {
            p->~record();
          }
          p = np;
        }
      }
      if (memory != nullptr) std::free(memory);
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
      using record_t = record<T, Act>;
      record_t * next() noexcept override { return nullptr; }
      record_t const * next() const noexcept override { return nullptr; }
      std::ostream & what(std::ostream & os) const noexcept override { return os << "terminator"; }
      record_t const * propagate([[maybe_unused]] Act act, [[maybe_unused]] index_t &) const noexcept override {
        return nullptr;
      }
    };

    template <typename T, typename Act> segment<T, Act>::segment(index_t n) noexcept
    : segment(
        static_cast<std::byte*>(aligned_alloc(record_alignment, pad_to_alignment(n))),
        pad_to_alignment(n)
    ) {
      [[maybe_unused]] auto p = new(*this) terminator<T,Act>();
      assert(is_aligned(p,record_alignment));
    } // alignas and pad to alignment

    template <typename T, typename Act>
    struct link: record<T, Act> {
      using record_t = record<T, Act>;
      using segment_t = segment<T, Act>;
      link() = delete;
      link(segment_t && segment) noexcept : segment(std::move(segment)) {}
      record_t * next() noexcept override { return segment.current; }
      record_t const * next() const noexcept override { return segment.current; }
      std::ostream & what(std::ostream & os) const noexcept override { return os << "link"; }
      record_t const * propagate([[maybe_unused]] Act act, [[maybe_unused]] index_t &) const noexcept override {
        return segment.current;
      }

      link<T, Act> * as_link() noexcept override { return this; }

      segment_t segment;
    };

    template <typename T, typename Act>
    segment<T, Act>::segment(index_t n, segment<T, Act> && next) noexcept
    : segment(
        static_cast<std::byte*>(aligned_alloc(record_alignment, pad_to_alignment(n))),
        pad_to_alignment(n)
    ) {
      if (next.memory != nullptr) {
        [[maybe_unused]] auto p = new(*this) link(std::move(next));
        assert(is_aligned(p,record_alignment));
      } else {
        [[maybe_unused]] auto p = new(*this) terminator<T,Act>();
        assert(is_aligned(p,record_alignment));
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
    index_t activations;

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
      auto result = new (*this) U(std::forward<Args>(args)...);
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
    void * record<T,Act>::operator new(size_t size, tape_t & tape) noexcept {
      auto result = record::operator new(size, tape.segment);
      if (result) return result;
      tape.segment = segment(std::max(segment_t::minimum_size, static_cast<index_t>(pad_to_alignment(std::max(sizeof(link<T, Act>), sizeof(terminator<T, Act>))) + pad_to_alignment(size))), std::move(tape.segment));
      result = record::operator new(size, tape.segment);
      assert(result != nullptr);
      return result;
    }

    // a non-terminal entry designed for allocation in a slab
    template <typename B, typename T, typename Act = T &>
    struct propagator : record<T,Act> {
      propagator() : record<T,Act>() {
      }
      inline record<T, Act> const * next() const noexcept override {
        return reinterpret_cast<record<T, Act> const *>(reinterpret_cast<std::byte const*>(this) + pad_to_alignment(sizeof(B)));
      }
      inline record<T, Act> * next() noexcept override {
        return reinterpret_cast<record<T, Act> *>(reinterpret_cast<std::byte*>(this) + pad_to_alignment(sizeof(B)));
      }
      std::ostream & what(std::ostream & os) const noexcept override {
        return os << type(*static_cast<B const *>(this));
      }

      const record<T,Act> * propagate(Act act, index_t & i) const noexcept override {
        reinterpret_cast<B const *>(this)->prop(act, i);
        return next(); // this shares the virtual function call dispatch, because here it isn't virtual.
      }
    };

    // a non-terminal entry designed for allocation in a slab, that produce a fixed number of activation records
    template <size_t Acts, typename B, typename T, typename Act = T*>
    struct static_propagator : propagator<B,T,Act> {
      static_propagator() : propagator<B,T,Act>() {}
      static constexpr size_t acts = Acts;
      constexpr index_t activation_records() const noexcept override {
        return acts;
      }
    };
  } // namespace detail

  using std::swap;
  using detail::swap;
} // namespace bad
