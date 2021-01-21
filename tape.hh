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
    BAD(HD,INLINE) bool is_aligned(const void * ptr, std::uintptr_t alignment) noexcept {
      auto iptr = reinterpret_cast<std::uintptr_t>(ptr);
      return !(iptr % alignment);
    }

    template <class T, class Act = T*>
    struct record;

    // TODO: cuda memory allocation
    // TODO: __host__ __device__ markers
    template <class T, class Act = T*>
    struct segment {
      using record_t = record<T,Act>;

      static constexpr index_t minimum_size = 65536;

      record_t * current;
      std::byte * memory;

      segment(const segment &) = delete;
      segment & operator=(segment const&) = delete;

    private:
      BAD(HD,INLINE) segment(std::byte * memory, index_t size) noexcept
      : current(reinterpret_cast<record<T>*>(memory + size))
      , memory(memory) {
      }

    public:
      BAD(HD,INLINE) segment() noexcept : current(nullptr), memory(nullptr) {};
      BAD_HD segment(index_t n) noexcept;
      BAD_HD segment(index_t n, segment && next) noexcept;
      BAD_HD segment(record_t * current, std::byte * memory) : current(current), memory(memory) {}
      BAD(HD,INLINE) segment(segment && rhs) noexcept
      : current(std::move(rhs.current))
      , memory(std::move(rhs.memory)) {
        rhs.current = nullptr;
        rhs.memory = nullptr;
      }

      BAD_HD ~segment() noexcept;

      BAD_HD segment & operator=(segment && rhs) noexcept;
    };

    template <class T, class Act>
    BAD(HD,INLINE) void swap(segment<T, Act> & a, segment<T, Act> & b) noexcept {
      using std::swap;
      swap(a.current, b.current);
      swap(a.memory, b.memory);
    }
  } // detail

  // forward declaration of tape
  template <class T, class Act = T*>
  struct tape;

  namespace detail {
    static constexpr BAD(HD,INLINE) index_t pad_to_alignment(index_t i) noexcept {
      return (i + record_alignment - 1) & record_mask;
    }

    template <class T, class Act = T*> struct link;

    // implement record
    template <class T, class Act>
    struct alignas(record_alignment) record {
      using tape_t = tape<T,Act>;
      using segment_t = segment<T, Act>;
      using act_t = Act;
      using record_t = record<T, Act>;

      BAD(HD,INLINE) record() noexcept {}
      // disable copy construction
      record(const record &) = delete;
      record & operator=(const record &) = delete;

      BAD_HD virtual record * next() noexcept = 0;
      BAD_HD virtual record const * next() const noexcept = 0;
      BAD_HD virtual ~record() noexcept {}
      BAD(MAYBE_UNUSED,HD) virtual std::ostream & what(std::ostream & os) const noexcept = 0;
      // now we have to add a bunch of stuff for doing propagation
      BAD_HD virtual index_t activation_records() const noexcept { return 0; }
      // should return the same answer as next
      BAD_HD virtual record const * propagate(Act act, index_t & i) const noexcept = 0;
      BAD_HD virtual link<T,Act> * as_link() noexcept { return nullptr; }

      // unlike usual, the result can be reached through the tape.
      BAD(MAYBE_UNUSED,HD) void * operator new(size_t size, tape_t & tape) noexcept;

      // used internally. returns null if the segment is out of room.
      BAD(MAYBE_UNUSED,HD) void * operator new(size_t size, segment_t & segment) noexcept;

      // we don't use the argument
      BAD_HD void operator delete(BAD_MAYBE_UNUSED void * data) noexcept {}

      // disable other new/delete forms:
      BAD_HD void * operator new  (size_t) = delete;
      BAD_HD void * operator new  (size_t, void *) noexcept = delete;
      BAD_HD void * operator new  (size_t, const std::nothrow_t &) = delete;
      BAD_HD void * operator new  (size_t, const std::align_val_t &, const std::nothrow_t &) = delete;
      BAD_HD void * operator new[](size_t) = delete;
      BAD_HD void * operator new[] (size_t, void *) noexcept = delete;
      BAD_HD void * operator new[](size_t, const std::nothrow_t &) = delete;
      BAD_HD void * operator new[](size_t, const std::align_val_t &, const std::nothrow_t &) = delete;
      BAD_HD void operator delete[](void *) noexcept = delete;
      BAD_HD void operator delete[](void *, size_t) noexcept = delete;
      BAD_HD void operator delete[](void *, std::align_val_t) noexcept = delete;
      BAD_HD void operator delete[](void *, size_t, std::align_val_t) noexcept = delete;
    };

    template <class T, class Act>
    inline std::ostream & operator << (std::ostream & os, const record<T, Act> & d) noexcept {
      return d.what(os);
    }

    template <class T, class Act>
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

    template <class T, class Act>
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

    template <class T, class Act>
    inline segment<T, Act> & segment<T, Act>::operator=(segment<T, Act> && rhs) noexcept {
      using std::swap;
      swap(*this,rhs);
      return *this;
    }

    template <class T, class Act = T*>
    struct terminator : record<T, Act> {
      using record_t = record<T, Act>;
      BAD(HD,INLINE) record_t * next() noexcept override { return nullptr; }
      BAD(HD,INLINE) record_t const * next() const noexcept override { return nullptr; }
      std::ostream & what(std::ostream & os) const noexcept override { return os << "terminator"; }
      BAD(HD,INLINE) record_t const * propagate(BAD_MAYBE_UNUSED Act act, BAD_MAYBE_UNUSED index_t &) const noexcept override {
        return nullptr;
      }
    };

    template <class T, class Act> segment<T, Act>::segment(index_t n) noexcept
    : segment(
        static_cast<std::byte*>(aligned_alloc(record_alignment, pad_to_alignment(n))),
        pad_to_alignment(n)
    ) {
      BAD_MAYBE_UNUSED auto p = new(*this) terminator<T,Act>();
      assert(is_aligned(p,record_alignment));
    } // alignas and pad to alignment

    template <class T, class Act>
    struct link: record<T, Act> {
      using record_t = record<T, Act>;
      using segment_t = segment<T, Act>;
      BAD_HD link() = delete;
      BAD_HD link(segment_t && segment) noexcept : segment(std::move(segment)) {}
      BAD(HD,INLINE) record_t * next() noexcept override { return segment.current; }
      BAD(HD,INLINE) record_t const * next() const noexcept override { return segment.current; }
      BAD_HD std::ostream & what(std::ostream & os) const noexcept override { return os << "link"; }
      BAD(HD,INLINE) record_t const * propagate(BAD_MAYBE_UNUSED Act act, BAD_MAYBE_UNUSED index_t &) const noexcept override {
        return segment.current;
      }

      BAD(HD,INLINE) link<T, Act> * as_link() noexcept override { return this; }
      segment_t segment;
    };

    template <class T, class Act>
    segment<T, Act>::segment(index_t n, segment<T, Act> && next) noexcept
    : segment(
        static_cast<std::byte*>(aligned_alloc(record_alignment, pad_to_alignment(n))),
        pad_to_alignment(n)
    ) {
      if (next.memory != nullptr) {
        BAD_MAYBE_UNUSED auto p = new(*this) link(std::move(next));
        assert(is_aligned(p,record_alignment));
      } else {
        BAD_MAYBE_UNUSED auto p = new(*this) terminator<T,Act>();
        assert(is_aligned(p,record_alignment));
      }
    }

    template <class A>
    struct intrusive_iterator : std::iterator<std::forward_iterator_tag, A> {
      using pointer = A*;
      using reference = A&;
      using const_pointer = std::add_const_t<pointer>;
      using const_reference = std::add_const_t<reference>;

      pointer p;

      BAD(HD,INLINE) intrusive_iterator() noexcept : p() {}
      BAD(HD,INLINE) intrusive_iterator(pointer p) noexcept : p(p) {}
      BAD(HD,INLINE) intrusive_iterator(const intrusive_iterator & rhs) noexcept : p(rhs.p) {}
      BAD(HD,INLINE) intrusive_iterator(intrusive_iterator &&  rhs) noexcept : p(std::move(rhs.p)) {}

      BAD(HD,INLINE) ~intrusive_iterator() noexcept {}

      BAD(HD,INLINE) constexpr bool operator == (const intrusive_iterator & rhs) const noexcept { return p == rhs.p; }
      BAD(HD,INLINE) constexpr bool operator != (const intrusive_iterator & rhs) const noexcept { return p != rhs.p; }

      BAD(HD,INLINE) constexpr reference operator *() const noexcept { return *p; }
      BAD(HD,INLINE) constexpr pointer operator -> () noexcept { return p; }
      BAD(HD,INLINE) intrusive_iterator & operator ++ () noexcept {
        assert(p != nullptr);
        p = p->next();
        return *this;
      }

      BAD(HD,INLINE) intrusive_iterator operator ++ (int) noexcept {
        assert(p != nullptr);
        auto q = p;
        p = p->next();
        return q;
      }

      BAD(HD,INLINE) constexpr pointer ptr() noexcept { return p; }
      BAD(HD,INLINE) constexpr const_pointer const_ptr() const noexcept { return p; }
      BAD(HD,INLINE) constexpr operator bool() const noexcept { return p != nullptr; }

      // template <class = std::enable_if_v<!std::is_const_v(A)> >
      BAD(HD,INLINE) constexpr operator intrusive_iterator<const A> () const noexcept { return p; }
    };

    template <class A>
    BAD(HD,INLINE) void swap (intrusive_iterator<A> & a, intrusive_iterator<A> & b) {
      using std::swap;
      swap(a.p,b.p);
    }
  } // detail

  // the workhorse
  template <class T, class Act>
  struct tape {
  protected:
    using segment_t = detail::segment<T,Act>;
    using record_t = detail::record<T, Act>;
  public:

    segment_t segment; // current segment
    index_t activations;

    using iterator = detail::intrusive_iterator<record_t>;
    using const_iterator = detail::intrusive_iterator<const record_t>;

    BAD_HD tape() noexcept : segment(), activations() {}
    BAD_HD tape(tape && rhs) noexcept : segment(std::move(rhs.segment)), activations(std::move(rhs.activations)) {}
    BAD_HD tape(const tape &) = delete;

    BAD(MAYBE_UNUSED,HD) tape & operator=(const tape &) = delete;
    BAD(MAYBE_UNUSED,HD) tape & operator=(tape && rhs) noexcept;

    // put more stuff in here
    template <class U, class ... Args>
    BAD(MAYBE_UNUSED,HD,FLATTEN) U & push(Args ... args) noexcept {
      static_assert(std::is_base_of_v<record_t, U>, "tape record not derived from record<T>");
      auto result = new (*this) U(std::forward<Args>(args)...);
      activations += result->activation_records();
      return *result;
    }

    BAD_HD iterator begin() noexcept { return segment.current; }
    BAD_HD constexpr iterator end() noexcept { return iterator(); }

    BAD_HD const_iterator cbegin() const noexcept { return segment.current; }
    BAD_HD constexpr const_iterator cend() noexcept { return const_iterator(); }
  };

  template <class T, class Act>
  BAD(HD,INLINE) void swap(tape<T, Act> & a, tape<T, Act> & b) noexcept {
    using std::swap;
    swap(a.segment, b.segment);
    swap(a.activations, b.activations);
  }

  template <typename T, typename Act>
  inline tape<T,Act> & tape<T,Act>::operator=(tape<T,Act> && rhs) noexcept {
    swap(*this,rhs);
    return *this;
  }

  namespace detail {
    template <class T, class Act>
    inline void * record<T,Act>::operator new(size_t size, tape_t & tape) noexcept {
      auto result = record::operator new(size, tape.segment);
      if (result) return result;
      tape.segment = segment(std::max(segment_t::minimum_size, static_cast<index_t>(pad_to_alignment(std::max(sizeof(link<T, Act>), sizeof(terminator<T, Act>))) + pad_to_alignment(size))), std::move(tape.segment));
      result = record::operator new(size, tape.segment);
      assert(result != nullptr);
      return result;
    }

    // a non-terminal entry designed for allocation in a slab
    template <class B, class T, class Act = T &>
    struct propagator : record<T,Act> {
      BAD(HD,INLINE) propagator() noexcept : record<T,Act>() {
      }
      BAD(HD,INLINE,FLATTEN) record<T, Act> const * next() const noexcept override {
        return reinterpret_cast<record<T, Act> const *>(reinterpret_cast<std::byte const*>(this) + pad_to_alignment(sizeof(B)));
      }
      BAD(HD,INLINE,FLATTEN) record<T, Act> * next() noexcept override {
        return reinterpret_cast<record<T, Act> *>(reinterpret_cast<std::byte*>(this) + pad_to_alignment(sizeof(B)));
      }
      BAD(MAYBE_UNUSED,HD,FLATTEN) std::ostream & what(std::ostream & os) const noexcept override {
        return os << type(*static_cast<B const *>(this));
      }

      BAD(HD,INLINE,FLATTEN) const record<T,Act> * propagate(Act act, index_t & i) const noexcept override {
        reinterpret_cast<B const *>(this)->prop(act, i);
        return next(); // this shares the virtual function call dispatch, because here it isn't virtual.
      }
    };

    // a non-terminal entry designed for allocation in a slab, that produces a fixed number of activation records
    template <size_t Acts, class B, class T, class Act = T*>
    struct static_propagator : propagator<B,T,Act> {
      BAD(HD,INLINE) static_propagator() noexcept : propagator<B,T,Act>() {}
      static constexpr size_t acts = Acts;
      BAD(HD,INLINE) constexpr index_t activation_records() const noexcept override {
        return acts;
      }
    };
  } // namespace detail

  using std::swap;
  using detail::swap;
} // namespace bad
