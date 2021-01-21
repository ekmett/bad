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
  static constexpr size_t no_index = static_cast<size_t>(-1);
  static constexpr size_t record_alignment = 16;
  static constexpr size_t record_mask = static_cast<size_t>(~0xf);

  namespace detail {
    BAD(HD,INLINE,CONST)
    bool is_aligned(BAD(NOESCAPE) const void * ptr, std::uintptr_t alignment) noexcept {
      auto iptr = reinterpret_cast<std::uintptr_t>(ptr);
      return !(iptr % alignment);
    }
  }

  template <class T, size_t alignment = record_alignment>
  struct aligned_allocator {
    using pointer = T*;
    using const_pointer = T const *;
    using void_pointer = void *;
    using const_void_pointer = void const *;
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    template <class U> struct rebind {
      using other = aligned_allocator<T, alignment>;
    };

    aligned_allocator() = default;

    template <class U>
    BAD(HD,INLINE,NOALIAS)
    constexpr aligned_allocator(BAD(NOESCAPE) const aligned_allocator<U,alignment> &) noexcept {}

    // TODO: remaining attributes
    // can't specify alloc_size(X,Y) because i can't specify the constant comes from the sizeof(T)
    BAD(NODISCARD,HD,INLINE,MALLOC,ASSUME_ALIGNED(alignment),RETURNS_NONNULL)
    T * allocate(size_t n) const noexcept {
      assert(n < std::numeric_limits<std::size_t>::max() / sizeof(T));
      T* result = static_cast<T*>(aligned_alloc(alignment,n));
      assert(result);
      return result;
    }

    BAD(HD,INLINE,NOALIAS)
    void deallocate(BAD(NOESCAPE) T *p, BAD(MAYBE_UNUSED) size_t n) noexcept {
      std::free(p);
    }

    BAD(HD,INLINE,NOALIAS)
    void deallocate(BAD(NOESCAPE) T *p) noexcept {
      std::free(p);
    }


    template <class U>
    BAD(HD,INLINE,CONST)
    bool operator ==(BAD(NOESCAPE) aligned_allocator<U,alignment> * rhs) const noexcept {
      return true;
    }

    template <class U>
    BAD(HD,INLINE,CONST)
    bool operator !=(BAD(NOESCAPE) aligned_allocator<U,alignment> * rhs) const noexcept {
      return false;
    }
  };

  using default_allocator = aligned_allocator<std::byte, record_alignment>;

  namespace detail {
    template <class T, class Act = T*, class Allocator = default_allocator>
    struct record;

    // TODO: cuda memory allocation
    // TODO: __host__ __device__ markers
    // assumes stateless allocator
    // that returns data with at least record_alignment alignment
    template <class T, class Act = T*, class Allocator = default_allocator>
    struct segment {
      using record_t = record<T,Act>;

      static constexpr size_t minimum_size = 65536;

      BAD(NO_UNIQUE_ADDRESS) Allocator allocator;
      record_t * current;
      std::byte * memory;

      segment(const segment &) = delete;
      segment & operator=(segment const&) = delete;

    private:
      BAD(HD,INLINE) segment(std::byte * memory, size_t size) noexcept
      : current(reinterpret_cast<record<T>*>(memory + size))
      , memory(memory) {
      }

    public:
      BAD(HD,INLINE,NOALIAS) segment() noexcept : current(nullptr), memory(nullptr) {};
      BAD(HD,NOALIAS) segment(size_t n) noexcept;
      BAD(HD,NOALIAS) segment(size_t n, segment && next) noexcept;
      BAD(HD,NOALIAS) segment(record_t * current, std::byte * memory) : current(current), memory(memory) {}
      BAD(HD,INLINE,NOALIAS) segment(segment && rhs) noexcept
      : current(std::move(rhs.current))
      , memory(std::move(rhs.memory)) {
        rhs.current = nullptr;
        rhs.memory = nullptr;
      }

      BAD(HD,NOALIAS) ~segment() noexcept;

      BAD(REINITIALIZES,HD,NOALIAS) segment & operator=(segment && rhs) noexcept;
    };

    template <class T, class Act, class Allocator>
    BAD(HD,INLINE,NOALIAS) void swap(BAD(NOESCAPE) segment<T, Act, Allocator> & a, BAD(NOESCAPE) segment<T, Act, Allocator> & b) noexcept {
      using std::swap;
      swap(a.current, b.current);
      swap(a.memory, b.memory);
    }
  } // detail

  // forward declaration of tape
  template <class T, class Act = T*,class Allocator = default_allocator>
  struct tape;

  namespace detail {
    BAD(HD,INLINE,CONST) static constexpr size_t pad_to_alignment(size_t i) noexcept {
      return (i + record_alignment - 1) & record_mask;
    }

    template <class T, class Act = T*, class Allocator = aligned_allocator<std::byte*, record_alignment>>
    struct link;

    // implement record
    template <class T, class Act, class Allocator>
    struct alignas(record_alignment) record {
      using tape_t = tape<T,Act,Allocator>;
      using segment_t = segment<T, Act, Allocator>;
      using act_t = Act;
      using record_t = record<T, Act, Allocator>;

      BAD(HD,INLINE,NOALIAS) record() noexcept {}

      // disable copy construction
      BAD(HD) record(const record &) = delete;

      BAD(HD) record & operator=(const record &) = delete;

      BAD(HD) virtual record * next() noexcept = 0;

      BAD(HD) virtual record const * next() const noexcept = 0;

      BAD(HD) virtual ~record() noexcept {}

      BAD(HD) virtual void what(BAD(NOESCAPE) std::ostream & os) const noexcept = 0;

      BAD(HD) virtual size_t activation_records() const noexcept { return 0; }

      BAD(HD,ASSUME_ALIGNED(record_alignment))
      virtual record const * propagate(Act act, BAD(NOESCAPE) size_t & i) const noexcept = 0;

      BAD(HD,ASSUME_ALIGNED(record_alignment),NOALIAS)
      virtual link<T,Act,Allocator> * as_link() noexcept { return nullptr; }

      // unlike usual, the result can be reached through the tape.
      BAD(MAYBE_UNUSED,HD,ALLOC_SIZE(1),MALLOC,ASSUME_ALIGNED(record_alignment))
      void * operator new(size_t size, BAD(NOESCAPE) tape_t & tape) noexcept;

      // used internally. returns null if the segment is out of room.
      BAD(MAYBE_UNUSED,HD,ALLOC_SIZE(1),MALLOC,ASSUME_ALIGNED(record_alignment))
      void * operator new(size_t size, BAD(NOESCAPE) segment_t & segment) noexcept;

      // we don't use the argument
      BAD(HD) void operator delete(BAD(MAYBE_UNUSED) void * data) noexcept {}

      // disable other new/delete forms:
      BAD(HD) void * operator new  (size_t) = delete;
      BAD(HD) void * operator new  (size_t, void *) noexcept = delete;
      BAD(HD) void * operator new  (size_t, const std::nothrow_t &) = delete;
      BAD(HD) void * operator new  (size_t, const std::align_val_t &, const std::nothrow_t &) = delete;
      BAD(HD) void * operator new[](size_t) = delete;
      BAD(HD) void * operator new[] (size_t, void *) noexcept = delete;
      BAD(HD) void * operator new[](size_t, const std::nothrow_t &) = delete;
      BAD(HD) void * operator new[](size_t, const std::align_val_t &, const std::nothrow_t &) = delete;
      BAD(HD) void operator delete[](void *) noexcept = delete;
      BAD(HD) void operator delete[](void *, size_t) noexcept = delete;
      BAD(HD) void operator delete[](void *, std::align_val_t) noexcept = delete;
      BAD(HD) void operator delete[](void *, size_t, std::align_val_t) noexcept = delete;
    };

    template <class T, class Act, class Allocator>
    inline std::ostream & operator << (std::ostream & os, BAD(NOESCAPE) const record<T, Act, Allocator> & d) noexcept {
      d.what(os);
      return os;
    }

    template <class T, class Act, class Allocator>
    void * record<T,Act,Allocator>::operator new(size_t t, BAD(NOESCAPE) segment_t & segment) noexcept {
      if (segment.memory == nullptr) return nullptr;
      std::byte * p BAD_ALIGN_VALUE(record_alignment) = reinterpret_cast<std::byte *>(segment.current);
      t = pad_to_alignment(t);
      if (p - segment.memory < t) return nullptr;
      p -= t;
      segment.current = reinterpret_cast<record_t*>(p);
      // requires c++20
      //return std::assume_aligned<record_alignment>(static_cast<void *>(p));
      return static_cast<void *>(p);
    }

    template <class T, class Act, class Allocator>
    segment<T, Act, Allocator>::~segment() noexcept {
      if (current != nullptr) {
        record<T, Act, Allocator> * p BAD_ALIGN_VALUE(record_alignment) = current;
        // this avoids building up a stack frame for each segment, but yeesh.
        while (p != nullptr) {
          record<T, Act, Allocator> * np BAD_ALIGN_VALUE(record_alignment) = p->next();
          link<T, Act, Allocator> * link BAD_ALIGN_VALUE(record_alignment)= p->as_link();
          if (link) {
            // we're going to become it
            segment<T, Act, Allocator> temp = std::move(link->segment);
            p->~record();
            allocator.deallocate(memory);
            memory = nullptr;
            current = nullptr;
            swap(*this,temp);
          } else {
            p->~record();
          }
          p = np;
        }
      }
      if (memory != nullptr) allocator.deallocate(memory);
      current = nullptr;
      memory = nullptr;
    }

    template <class T, class Act, class Allocator>
    inline segment<T, Act, Allocator> & segment<T, Act, Allocator>::operator=(segment<T, Act, Allocator> && rhs) noexcept {
      using std::swap;
      swap(*this,rhs);
      return *this;
    }

    template <class T, class Act = T*, class Allocator = default_allocator>
    struct terminator : record<T, Act, Allocator> {
      using record_t = record<T, Act, Allocator>;
      BAD(HD,INLINE,CONST) record_t * next() noexcept override { return nullptr; }
      BAD(HD,INLINE,CONST) record_t const * next() const noexcept override { return nullptr; }
      BAD(HD) void what(BAD(NOESCAPE) std::ostream & os) const noexcept override { os << "terminator"; }
      BAD(HD,INLINE,CONST) record_t const * propagate(BAD(MAYBE_UNUSED) Act act, BAD(MAYBE_UNUSED,NOESCAPE) size_t &) const noexcept override {
        return nullptr;
      }
    };

    template <class T, class Act, class Allocator> segment<T, Act, Allocator>::segment(size_t n) noexcept
    : segment(
        static_cast<std::byte*>(aligned_alloc(record_alignment, pad_to_alignment(n))),
        pad_to_alignment(n)
    ) {
      BAD_MAYBE_UNUSED auto p = new(*this) terminator<T,Act, Allocator>();
      assert(is_aligned(p,record_alignment));
    } // alignas and pad to alignment

    template <class T, class Act, class Allocator>
    struct link: record<T, Act, Allocator> {
      using record_t = record<T, Act, Allocator>;
      using segment_t = segment<T, Act, Allocator>;
      BAD(HD) link() = delete;
      BAD(HD,NOALIAS) link(segment_t && segment) noexcept : segment(std::move(segment)) {}

      BAD(HD,INLINE,PURE) record_t * next() noexcept override { return segment.current; }
      BAD(HD,INLINE,PURE) record_t const * next() const noexcept override { return segment.current; }
      BAD(HD) void what(BAD(NOESCAPE) std::ostream & os) const noexcept override { os << "link"; }
      BAD(HD,INLINE,PURE) record_t const * propagate(BAD(MAYBE_UNUSED) Act act, BAD(MAYBE_UNUSED,NOESCAPE) size_t &) const noexcept override {
        return segment.current;
      }
      BAD(HD,INLINE,CONST) link<T, Act, Allocator> * as_link() noexcept override { return this; }

      segment_t segment;
    };

    template <class T, class Act, class Allocator>
    segment<T, Act, Allocator>::segment(size_t n, segment<T, Act, Allocator> && next) noexcept
    : segment(
        static_cast<std::byte*>(aligned_alloc(record_alignment, pad_to_alignment(n))),
        pad_to_alignment(n)
    ) {
      if (next.memory != nullptr) {
        BAD_MAYBE_UNUSED auto p = new(*this) link(std::move(next));
        assert(is_aligned(p,record_alignment));
      } else {
        BAD_MAYBE_UNUSED auto p = new(*this) terminator<T,Act,Allocator>();
        assert(is_aligned(p,record_alignment));
      }
    }

    template <class T, class Act = T*, class Allocator = default_allocator>
    struct const_record_iterator : std::iterator<std::forward_iterator_tag, record<T,Act,Allocator> const> {
      using pointer = record<T,Act,Allocator> const *;
      using reference = record<T,Act,Allocator> const &;
      using const_pointer = pointer;
      using const_reference = reference;

      pointer p;

      BAD(HD,INLINE,NOALIAS) const_record_iterator() noexcept : p() {}
      BAD(HD,INLINE,NOALIAS) const_record_iterator(pointer p) noexcept : p(p) {}
      BAD(HD,INLINE,NOALIAS) const_record_iterator(const const_record_iterator & rhs) noexcept : p(rhs.p) {}
      BAD(HD,INLINE,NOALIAS) const_record_iterator(const_record_iterator &&  rhs) noexcept : p(std::move(rhs.p)) {}

      BAD(HD,INLINE,PURE) constexpr bool operator == (const const_record_iterator & rhs) const noexcept { return p == rhs.p; }
      BAD(HD,INLINE,PURE) constexpr bool operator != (const const_record_iterator & rhs) const noexcept { return p != rhs.p; }

      BAD(HD,INLINE,PURE,ASSUME_ALIGNED(record_alignment)) constexpr reference operator *() const noexcept { return *p; }
      BAD(HD,INLINE,PURE,ASSUME_ALIGNED(record_alignment)) constexpr pointer operator -> () noexcept { return p; }
      BAD(HD,INLINE,NOALIAS) const_record_iterator & operator ++ () noexcept {
        assert(p != nullptr);
        p = p->next();
        return *this;
      }

      BAD(HD,INLINE,NOALIAS) const_record_iterator operator ++ (int) noexcept {
        assert(p != nullptr);
        auto q = p;
        p = p->next();
        return q;
      }

      BAD(HD,INLINE,PURE,ASSUME_ALIGNED(record_alignment)) constexpr pointer ptr() noexcept { return p; }
      BAD(HD,INLINE,PURE,ASSUME_ALIGNED(record_alignment)) constexpr const_pointer const_ptr() const noexcept { return p; }
      BAD(HD,INLINE,PURE) constexpr operator bool() const noexcept { return p != nullptr; }
    };

    template <class T, class Act, class Allocator>
    BAD(HD,INLINE,NOALIAS) void swap (BAD(NOESCAPE) const_record_iterator<T,Act,Allocator> & a, BAD(NOESCAPE) const_record_iterator<T,Act,Allocator> & b) {
      using std::swap;
      swap(a.p,b.p);
    }

    template <class T, class Act = T*, class Allocator = default_allocator>
    struct record_iterator : std::iterator<std::forward_iterator_tag, record<T,Act,Allocator>> {
      using pointer = record<T,Act,Allocator>*;
      using reference = record<T,Act,Allocator>&;
      using const_pointer = record<T,Act,Allocator> const *;
      using const_reference = record<T,Act,Allocator> const &;

      pointer p;

      BAD(HD,INLINE,NOALIAS) record_iterator() noexcept : p() {}
      BAD(HD,INLINE,NOALIAS) record_iterator(pointer p) noexcept : p(p) {}
      BAD(HD,INLINE,NOALIAS) record_iterator(const record_iterator & rhs) noexcept : p(rhs.p) {}
      BAD(HD,INLINE,NOALIAS) record_iterator(record_iterator &&  rhs) noexcept : p(std::move(rhs.p)) {}

      BAD(HD,INLINE,PURE) constexpr bool operator == (const record_iterator & rhs) const noexcept { return p == rhs.p; }
      BAD(HD,INLINE,PURE) constexpr bool operator != (const record_iterator & rhs) const noexcept { return p != rhs.p; }

      BAD(HD,INLINE,PURE,ASSUME_ALIGNED(record_alignment)) constexpr reference operator *() const noexcept { return *p; }
      BAD(HD,INLINE,PURE,ASSUME_ALIGNED(record_alignment)) constexpr pointer operator -> () noexcept { return p; }
      BAD(HD,INLINE,NOALIAS) record_iterator & operator ++ () noexcept {
        assert(p != nullptr);
        p = p->next();
        return *this;
      }

      BAD(HD,INLINE,NOALIAS) record_iterator operator ++ (int) noexcept {
        assert(p != nullptr);
        auto q = p;
        p = p->next();
        return q;
      }

      BAD(HD,INLINE,PURE,ASSUME_ALIGNED(record_alignment)) constexpr pointer ptr() noexcept { return p; }
      BAD(HD,INLINE,PURE,ASSUME_ALIGNED(record_alignment)) constexpr const_pointer const_ptr() const noexcept { return p; }
      BAD(HD,INLINE,PURE) constexpr operator bool() const noexcept { return p != nullptr; }

      BAD(HD,INLINE,PURE) constexpr operator const_record_iterator<T,Act,Allocator> () const noexcept { return p; }
    };

    template <class T,class Act,class Allocator>
    BAD(HD,INLINE,NOALIAS) void swap (BAD(NOESCAPE) record_iterator<T,Act,Allocator> & a, BAD(NOESCAPE) record_iterator<T,Act,Allocator> & b) {
      using std::swap;
      swap(a.p,b.p);
    }


  } // detail

  // the workhorse
  template <class T, class Act, class Allocator>
  struct tape {
  protected:
    using segment_t = detail::segment<T,Act,Allocator>;
    using record_t = detail::record<T,Act,Allocator>;
  public:
    segment_t segment; // current segment
    size_t activations;

    using iterator = detail::record_iterator<T,Act,Allocator>;
    using const_iterator = detail::const_record_iterator<T,Act,Allocator>;

    BAD(HD,NOALIAS) tape() noexcept : segment(), activations() {}
    BAD(HD,NOALIAS) tape(tape && rhs) noexcept : segment(std::move(rhs.segment)), activations(std::move(rhs.activations)) {}
    BAD(HD) tape(const tape &) = delete;

    BAD(MAYBE_UNUSED,HD) tape & operator=(const tape &) = delete;
    BAD(REINITIALIZES,MAYBE_UNUSED,HD,NOALIAS) tape & operator=(tape && rhs) noexcept;

    // put more stuff in here
    template <class U, class ... Args>
    BAD(MAYBE_UNUSED,HD,FLATTEN,NOALIAS) U & push(Args ... args) noexcept {
      static_assert(std::is_base_of_v<record_t, U>, "tape record not derived from record<T>");
      U * result BAD_ALIGN_VALUE(record_alignment) = new (*this) U(std::forward<Args>(args)...);
      activations += result->activation_records();
      return *result;
    }

    BAD(HD,PURE) iterator begin() noexcept { return segment.current; }
    BAD(HD,CONST) constexpr iterator end() noexcept { return iterator(); }

    BAD(HD,PURE) const_iterator cbegin() const noexcept { return segment.current; }
    BAD(HD,CONST) constexpr const_iterator cend() noexcept { return const_iterator(); }
  };

  template <class T, class Act, class Allocator>
  BAD(HD,INLINE,NOALIAS) void swap(BAD(NOESCAPE) tape<T,Act,Allocator> & a, BAD(NOESCAPE) tape<T,Act,Allocator> & b) noexcept {
    using std::swap;
    swap(a.segment, b.segment);
    swap(a.activations, b.activations);
  }

  template <class T, class Act,class Allocator>
  inline tape<T,Act,Allocator> & tape<T,Act,Allocator>::operator=(tape<T,Act,Allocator> && rhs) noexcept {
    swap(*this,rhs);
    return *this;
  }

  namespace detail {
    template <class T, class Act, class Allocator>
    inline void * record<T,Act,Allocator>::operator new(size_t size, BAD(NOESCAPE) tape_t & tape) noexcept {
      auto result = record::operator new(size, tape.segment);
      if (result) return result;
      tape.segment = segment(std::max(segment_t::minimum_size, static_cast<size_t>(pad_to_alignment(std::max(sizeof(link<T, Act, Allocator>), sizeof(terminator<T, Act, Allocator>))) + pad_to_alignment(size))), std::move(tape.segment));
      result = record::operator new(size, tape.segment);
      assert(result != nullptr);
      return result;
    }

    // a non-terminal entry designed for allocation in a slab
    template <class B, class T, class Act = T &, class Allocator = default_allocator>
    struct propagator : record<T,Act,Allocator> {
      using record_t = record<T,Act,Allocator>;

      BAD(HD,INLINE,NOALIAS)
      propagator() noexcept : record<T,Act,Allocator>() {}

      BAD(HD,INLINE,FLATTEN,CONST,ASSUME_ALIGNED(record_alignment))
      record_t const * next() const noexcept override {
        return reinterpret_cast<record_t const *>(reinterpret_cast<std::byte const*>(this) + pad_to_alignment(sizeof(B)));
      }

      BAD(HD,INLINE,FLATTEN,CONST,ASSUME_ALIGNED(record_alignment))
      record_t * next() noexcept override {
        return reinterpret_cast<record_t *>(reinterpret_cast<std::byte*>(this) + pad_to_alignment(sizeof(B)));
      }

      BAD(HD,FLATTEN)
      void what(BAD(NOESCAPE) std::ostream & os) const noexcept override {
        os << type(*static_cast<B const *>(this));
      }

      BAD(HD,INLINE,FLATTEN,ASSUME_ALIGNED(record_alignment))
      const record_t * propagate(Act act, BAD(NOESCAPE) size_t & i) const noexcept override {
        reinterpret_cast<B const *>(this)->prop(act, i);
        return next(); // this shares the virtual function call dispatch, because here it isn't virtual.
      }
    };

    // a non-terminal entry designed for allocation in a slab, that produces a fixed number of activation records
    template <size_t Acts, class B, class T, class Act = T*, class Allocator = default_allocator>
    struct static_propagator : propagator<B,T,Act,Allocator> {

      BAD(HD,INLINE,NOALIAS)
      static_propagator() noexcept : propagator<B,T,Act,Allocator>() {}

      static constexpr size_t acts = Acts;

      BAD(HD,INLINE,CONST)
      constexpr size_t activation_records() const noexcept override {
        return acts;
      }
    };
  } // namespace detail

  using std::swap;
  using detail::swap;
} // namespace bad
