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
#include "types.hh"
#include "memory.hh"

namespace bad {
  namespace tapes {
    namespace common {}
    using namespace common;
    namespace api {
      using namespace common;
    }
    using namespace api;
  }
  using namespace tapes::common;
}

namespace bad::tapes {
  using namespace memory::api;
  static constexpr size_t no_index = static_cast<size_t>(-1);

  namespace common {
    template <class T, class Act = T*, class Allocator = default_allocator>
    struct record;

    template <class T, class Act = T*,class Allocator = default_allocator>
    struct tape;
  }

  /// Assumes stateless allocator that returns data with at least record_alignment alignment
  template <class T, class Act = T*, class Allocator = default_allocator>
  struct segment {
    using record_t = record<T,Act,Allocator>;

    static constexpr size_t minimum_size = 65536;

    BAD(no_unique_address) Allocator allocator;
    record_t * current;
    std::byte * memory;

    segment(const segment &) = delete;
    segment & operator=(segment const&) = delete;

  private:
    BAD(hd,inline) segment(std::byte * memory, size_t size) noexcept
    : current(reinterpret_cast<record<T>*>(memory + size))
    , memory(memory) {
    }

  public:
    BAD(hd,inline,noalias) segment() noexcept : current(nullptr), memory(nullptr) {};
    BAD(hd,noalias) segment(size_t n) noexcept;
    BAD(hd,noalias) segment(size_t n, segment && next) noexcept;
    BAD(hd,noalias) segment(record_t * current, std::byte * memory) : current(current), memory(memory) {}
    BAD(hd,inline,noalias) segment(segment && rhs) noexcept
    : current(std::move(rhs.current))
    , memory(std::move(rhs.memory)) {
      rhs.current = nullptr;
      rhs.memory = nullptr;
    }

    BAD(hd,noalias) ~segment() noexcept;

    BAD(reinitializes,hd,noalias) segment & operator=(segment && rhs) noexcept;

  };

  template <class T, class Act, class Allocator>
  BAD(hd,inline,noalias) void swap(
    BAD(noescape) segment<T, Act, Allocator> & a,
    BAD(noescape) segment<T, Act, Allocator> & b
  ) noexcept {
    using std::swap;
    swap(a.current, b.current);
    swap(a.memory, b.memory);
  }

  BAD(hd,inline,const) static constexpr size_t pad_to_alignment(size_t i) noexcept {
    return (i + record_alignment - 1) & record_mask;
  }

  template <class T, class Act = T*, class Allocator = aligned_allocator<std::byte*, record_alignment>>
  struct link;

  namespace common {

    // implement record
    template <class T, class Act, class Allocator>
    struct alignas(record_alignment) record {
      using tape_t = tape<T,Act,Allocator>;
      using segment_t = segment<T, Act, Allocator>;
      using act_t = Act;
      using record_t = record<T, Act, Allocator>;
  
      BAD(hd,inline,noalias)
      record() noexcept {}
  
      // disable copy construction
      BAD(hd)
      record(const record &) = delete;
  
      BAD(hd)
      record & operator=(const record &) = delete;
  
      BAD(hd)
      virtual record * next() noexcept = 0;
  
      BAD(hd)
      virtual record const * next() const noexcept = 0;
  
      BAD(hd)
      virtual ~record() noexcept {}
  
      BAD(hd)
      virtual void what(BAD(noescape) std::ostream & os) const noexcept = 0;
  
      BAD(hd)
      virtual size_t activation_records() const noexcept { return 0; }
  
      BAD(hd,assume_aligned(record_alignment))
      virtual record const * propagate(Act act, BAD(noescape) size_t & i) const noexcept = 0;
  
      BAD(hd,assume_aligned(record_alignment),noalias)
      virtual link<T,Act,Allocator> const * as_link() const noexcept { return nullptr; }

      BAD(hd,assume_aligned(record_alignment),noalias)
      virtual link<T,Act,Allocator> * as_link() noexcept { return nullptr; }
  
      // unlike usual, the result can be reached through the tape.
      BAD(maybe_unused,hd,alloc_size(1),malloc,assume_aligned(record_alignment))
      void * operator new(size_t size, BAD(noescape) tape_t & tape) noexcept;
  
      // used internally. returns null if the segment is out of room.
      BAD(maybe_unused,hd,alloc_size(1),malloc,assume_aligned(record_alignment))
      void * operator new(size_t size, BAD(noescape) segment_t & segment) noexcept;
  
      // we don't use the argument
      BAD(hd)
      void operator delete(BAD(maybe_unused) void * data) noexcept {}
  
      // disable other new/delete forms:
      BAD(hd) void * operator new  (size_t) = delete;
      BAD(hd) void * operator new  (size_t, void *) noexcept = delete;
      BAD(hd) void * operator new  (size_t, const std::nothrow_t &) = delete;
      BAD(hd) void * operator new  (size_t, const std::align_val_t &, const std::nothrow_t &) = delete;
      BAD(hd) void * operator new[](size_t) = delete;
      BAD(hd) void * operator new[](size_t, void *) noexcept = delete;
      BAD(hd) void * operator new[](size_t, const std::nothrow_t &) = delete;
      BAD(hd) void * operator new[](size_t, const std::align_val_t &, const std::nothrow_t &) = delete;
      BAD(hd) void operator delete[](void *) noexcept = delete;
      BAD(hd) void operator delete[](void *, size_t) noexcept = delete;
      BAD(hd) void operator delete[](void *, std::align_val_t) noexcept = delete;
      BAD(hd) void operator delete[](void *, size_t, std::align_val_t) noexcept = delete;
    };
  
    template <class T, class Act, class Allocator>
    inline std::ostream & operator << (
      std::ostream & os,
      BAD(noescape) const record<T, Act, Allocator> & d
    ) noexcept {
      d.what(os);
      return os;
    }
  
    template <class T, class Act, class Allocator>
    void * record<T,Act,Allocator>::operator new(size_t t, BAD(noescape) segment_t & segment) noexcept {
      if (segment.memory == nullptr) return nullptr;
      std::byte * p BAD(align_value(record_alignment)) = reinterpret_cast<std::byte *>(segment.current);
      t = pad_to_alignment(t);
      if (p - segment.memory < ptrdiff_t(t)) return nullptr;
      p -= t;
      segment.current = reinterpret_cast<record_t*>(p);
      // requires c++20
      //return std::assume_aligned<record_alignment>(static_cast<void *>(p));
      return static_cast<void *>(p);
    }
  }

  template <class T, class Act, class Allocator>
  segment<T, Act, Allocator>::~segment() noexcept {
    if (current != nullptr) {
      record<T, Act, Allocator> * p BAD(align_value(record_alignment)) = current;
      // this avoids building up a stack frame for each segment, but yeesh.
      while (p != nullptr) {
        record<T, Act, Allocator> * np BAD(align_value(record_alignment)) = p->next();
        link<T, Act, Allocator> * link BAD(align_value(record_alignment)) = p->as_link();
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
  inline segment<T, Act, Allocator> & segment<T, Act, Allocator>::operator=(
    segment<T, Act, Allocator> && rhs
  ) noexcept {
    using std::swap;
    swap(*this,rhs);
    return *this;
  }

  template <class T, class Act = T*, class Allocator = default_allocator>
  struct terminator : record<T, Act, Allocator> {
    using record_t = record<T, Act, Allocator>;

    BAD(hd,inline,const)
    record_t * next() noexcept override {
      return nullptr;
    }

    BAD(hd,inline,const)
    record_t const * next() const noexcept override {
      return nullptr;
    }

    BAD(hd)
    void what(BAD(noescape) std::ostream & os) const noexcept override {
      os << "terminator";
    }

    BAD(hd,inline,const)
    record_t const * propagate(
      BAD(maybe_unused) Act act,
      BAD(maybe_unused,noescape) size_t &
    ) const noexcept override {
      return nullptr;
    }
  };

  template <class T, class Act, class Allocator> segment<T, Act, Allocator>::segment(size_t n) noexcept
  : segment(
      static_cast<std::byte*>(aligned_alloc(record_alignment, pad_to_alignment(n))),
      pad_to_alignment(n)
  ) {
    BAD(maybe_unused) auto p = new(*this) terminator<T,Act, Allocator>();
    assert(is_aligned(p,record_alignment));
    // done. p is reachable through through current.
  }

  template <class T, class Act, class Allocator>
  struct link: record<T, Act, Allocator> {
    using record_t = record<T, Act, Allocator>;
    using segment_t = segment<T, Act, Allocator>;

    BAD(hd)
    link() = delete;

    BAD(hd,noalias)
    link(segment_t && segment) noexcept
    : segment(std::move(segment)) {}

    BAD(hd,inline,pure)
    record_t * next() noexcept override {
      return segment.current;
    }

    BAD(hd,inline,pure)
    record_t const * next() const noexcept override {
      return segment.current;
    }

    BAD(hd)
    void what(BAD(noescape) std::ostream & os) const noexcept override {
      os << "link";
    }

    BAD(hd,inline,pure)
    record_t const * propagate(
      BAD(maybe_unused) Act act,
      BAD(maybe_unused,noescape) size_t &
    ) const noexcept override {
      return segment.current;
    }

    BAD(hd,inline,const)
    link<T, Act, Allocator> * as_link() noexcept override {
      return this;
    }

    BAD(hd,inline,const)
    link<T, Act, Allocator> const * as_link() const noexcept override {
      return this;
    }

    segment_t segment;
  };

  template <class T, class Act, class Allocator>
  segment<T, Act, Allocator>::segment(size_t n, segment<T, Act, Allocator> && next) noexcept
  : segment(
      static_cast<std::byte*>(aligned_alloc(record_alignment, pad_to_alignment(n))),
      pad_to_alignment(n)
  ) {
    if (next.memory != nullptr) {
      BAD(maybe_unused) auto p = new(*this) link(std::move(next));
      assert(is_aligned(p,record_alignment));
    } else {
      BAD(maybe_unused) auto p = new(*this) terminator<T,Act,Allocator>();
      assert(is_aligned(p,record_alignment));
    }
  }

  namespace common {

    // the workhorse
    template <class T, class Act, class Allocator>
    struct tape {
    protected:
      using segment_t = segment<T,Act,Allocator>;
      using record_t = record<T,Act,Allocator>;
    public:
      segment_t segment; // current segment
      size_t activations;
  
      BAD(hd,noalias)
      tape() noexcept
      : segment(), activations() {}
  
      BAD(hd,noalias)
      tape(tape && rhs) noexcept
      : segment(std::move(rhs.segment)), activations(std::move(rhs.activations)) {}
  
      BAD(hd)
      tape(const tape &) = delete;
  
      BAD(maybe_unused,hd)
      tape & operator=(const tape &) = delete;
      
      BAD(reinitializes,maybe_unused,hd,noalias)
      tape & operator=(tape && rhs) noexcept;
  
      // put more stuff in here
      template <class U, class ... Args>
      BAD(maybe_unused,hd,flatten,noalias)
      U & push(Args ... args) noexcept {
        static_assert(std::is_base_of_v<record_t, U>, "tape record not derived from record<T>");
        U * result BAD(align_value(record_alignment)) = new (*this) U(std::forward<Args>(args)...);
        activations += result->activation_records();
        return *result;
      }
  
      struct const_iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = record_t const;
        using pointer = value_type *;
        using reference = value_type &;
        using const_pointer = pointer;
        using const_reference = reference;
  
        pointer p;
  
        BAD(hd,inline,noalias)
        const_iterator() noexcept : p() {}
  
        BAD(hd,inline,noalias)
        const_iterator(pointer p) noexcept : p(p) {}
  
        BAD(hd,inline,noalias)
        const_iterator(const const_iterator & rhs) noexcept : p(rhs.p) {}
  
        BAD(hd,inline,noalias)
        const_iterator(const_iterator &&  rhs) noexcept : p(std::move(rhs.p)) {}
  
        BAD(hd,inline,pure)
        friend constexpr bool operator == (const_iterator lhs, const_iterator rhs) noexcept {
          return lhs.p == rhs.p;
        }
  
        BAD(hd,inline,pure)
        friend constexpr bool operator != (const_iterator lhs, const_iterator rhs) noexcept {
          return lhs.p != rhs.p;
        }
  
        BAD(hd,inline,pure,assume_aligned(record_alignment))
        constexpr reference operator *() const noexcept { return *p; }
  
        BAD(hd,inline,pure,assume_aligned(record_alignment))
        constexpr pointer operator -> () noexcept { return p; }
  
        BAD(hd,inline,noalias)
        const_iterator & operator ++ () noexcept {
          assert(p != nullptr);
          p = p->next();
          return *this;
        }
  
        BAD(hd,inline,noalias)
        const_iterator operator ++ (int) noexcept {
          assert(p != nullptr);
          auto q = p;
          p = p->next();
          return q;
        }
  
        BAD(hd,inline,pure,assume_aligned(record_alignment))
        constexpr pointer ptr() noexcept { return p; }
  
        BAD(hd,inline,pure,assume_aligned(record_alignment))
        constexpr const_pointer const_ptr() const noexcept { return p; }
  
        BAD(hd,inline,pure)
        constexpr operator bool() const noexcept {
          return p != nullptr;
        }
      };
  
      struct iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = record_t;
        using pointer = value_type *;
        using reference = value_type &;
        using const_pointer = value_type const *;
        using const_reference = value_type const &;
  
        pointer p;
  
        BAD(hd,inline,noalias)
        iterator() noexcept : p() {}
  
        BAD(hd,inline,noalias)
        iterator(pointer p) noexcept : p(p) {}
  
        BAD(hd,inline,noalias)
        iterator(const iterator & rhs) noexcept : p(rhs.p) {}
  
        BAD(hd,inline,noalias)
        iterator(iterator &&  rhs) noexcept : p(std::move(rhs.p)) {}
  
        BAD(hd,inline,pure)
        friend constexpr bool operator == (iterator lhs, iterator rhs) noexcept {
          return lhs.p == rhs.p;
        }
  
        BAD(hd,inline,pure)
        friend constexpr bool operator != (iterator lhs, iterator rhs) noexcept {
          return lhs.p != rhs.p;
        }
  
        BAD(hd,inline,pure,assume_aligned(record_alignment))
        constexpr reference operator *() const noexcept {
          return *p;
        }
  
        BAD(hd,inline,pure,assume_aligned(record_alignment))
        constexpr pointer operator -> () noexcept {
          return p;
        }
  
        BAD(hd,inline,noalias)
        iterator & operator ++ () noexcept {
          assert(p != nullptr);
          p = p->next();
          return *this;
        }
  
        BAD(hd,inline,noalias)
        iterator operator ++ (int) noexcept {
          assert(p != nullptr);
          auto q = p;
          p = p->next();
          return q;
        }
  
        BAD(hd,inline,pure,assume_aligned(record_alignment))
        constexpr pointer ptr() noexcept {
          return p;
        }
  
        BAD(hd,inline,pure,assume_aligned(record_alignment))
        constexpr const_pointer const_ptr() const noexcept {
          return p;
        }
  
        BAD(hd,inline,pure)
        constexpr operator bool() const noexcept {
          return p != nullptr;
        }
  
        BAD(hd,inline,pure)
        constexpr operator const_iterator () const noexcept {
          return p;
        }
      };
  
      BAD(hd,pure)
      iterator begin() noexcept {
        return segment.current;
      }
  
      BAD(hd,const)
      constexpr iterator end() noexcept {
        return iterator();
      }
  
      BAD(hd,pure)
      const_iterator begin() const noexcept {
        return segment.current;
      }
  
      BAD(hd,const)
      constexpr const_iterator end() const noexcept {
        return const_iterator();
      }
  
      BAD(hd,pure)
      const_iterator cbegin() const noexcept {
        return segment.current;
      }
  
      BAD(hd,const)
      constexpr const_iterator cend() noexcept {
        return const_iterator();
      }
    };
  
    template <class T, class Act, class Allocator>
    BAD(hd,inline,noalias)
    void swap(
      BAD(noescape) tape<T,Act,Allocator> & a,
      BAD(noescape) tape<T,Act,Allocator> & b
    ) noexcept {
      using std::swap;
      swap(a.segment, b.segment);
      swap(a.activations, b.activations);
    }
  
    template <class T, class Act,class Allocator>
    inline tape<T,Act,Allocator> & tape<T,Act,Allocator>::operator=(
      tape<T,Act,Allocator> && rhs
    ) noexcept {
      swap(*this,rhs);
      return *this;
    }
  
    template <class T, class Act, class Allocator>
    inline void * record<T,Act,Allocator>::operator new(
      size_t size,
      BAD(noescape) tape_t & tape
    ) noexcept {
      auto result = record::operator new(size, tape.segment);
      if (result) return result;
      tape.segment = segment(
        std::max<size_t>(
          segment_t::minimum_size,
          pad_to_alignment(size) + pad_to_alignment(
            std::max<size_t>(sizeof(link<T, Act, Allocator>), sizeof(terminator<T, Act, Allocator>))
          )
        ),
        std::move(tape.segment)
      );
      result = record::operator new(size, tape.segment);
      assert(result != nullptr);
      return result;
    }
  
    // a non-terminal entry designed for allocation in a slab
    template <class B, class T, class Act = T &, class Allocator = default_allocator>
    struct propagator : record<T,Act,Allocator> {
      using record_t = record<T,Act,Allocator>;
  
      BAD(hd,inline,noalias)
      propagator() noexcept : record<T,Act,Allocator>() {}
  
      BAD(hd,inline,flatten,const,assume_aligned(record_alignment))
      record_t const * next() const noexcept override {
        return reinterpret_cast<record_t const *>(reinterpret_cast<std::byte const*>(this) + pad_to_alignment(sizeof(B)));
      }
  
      BAD(hd,inline,flatten,const,assume_aligned(record_alignment))
      record_t * next() noexcept override {
        return reinterpret_cast<record_t *>(reinterpret_cast<std::byte*>(this) + pad_to_alignment(sizeof(B)));
      }
  
      BAD(hd,flatten)
      void what(BAD(noescape) std::ostream & os) const noexcept override {
        os << type(*static_cast<B const *>(this));
      }
  
      BAD(hd,inline,flatten,assume_aligned(record_alignment))
      const record_t * propagate(Act act, BAD(noescape) size_t & i) const noexcept override {
        reinterpret_cast<B const *>(this)->prop(act, i);
        return next(); // this shares the virtual function call dispatch, because here it isn't virtual.
      }
    };
  
    // a non-terminal entry designed for allocation in a slab, that produces a fixed number of activation records
    template <size_t Acts, class B, class T, class Act = T*, class Allocator = default_allocator>
    struct static_propagator : propagator<B,T,Act,Allocator> {
  
      BAD(hd,inline,noalias)
      static_propagator() noexcept : propagator<B,T,Act,Allocator>() {}
  
      static constexpr size_t acts = Acts;
  
      BAD(hd,inline,const)
      constexpr size_t activation_records() const noexcept override {
        return acts;
      }
    };
  
    template <class T, class Act, class Allocator>
    BAD(hd,inline,noalias)
    void swap (
      BAD(noescape) typename tape<T,Act,Allocator>::iterator & a,
      BAD(noescape) typename tape<T,Act,Allocator>::iterator & b
    ) {
      using std::swap;
      swap(a.p,b.p);
    }
  
    template <class T,class Act,class Allocator>
    BAD(hd,inline,noalias)
    void swap (
      BAD(noescape) typename tape<T,Act,Allocator>::const_iterator & a,
      BAD(noescape) typename tape<T,Act,Allocator>::const_iterator & b
    ) {
      using std::swap;
      swap(a.p,b.p);
    }
  }
}

namespace bad {
  using namespace tapes::common;
}
