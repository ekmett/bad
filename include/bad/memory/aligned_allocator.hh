#ifndef BAD_MEMORY_ALIGNED_ALLOCATOR_HH
#define BAD_MEMORY_ALIGNED_ALLOCATOR_HH

#include <cstdint>
#include <limits>
#include <type_traits>
#include <cstdlib>
#include <memory>

#include "bad/attributes.hh"

/// \file
/// \brief aligned memory allocation

namespace bad::memory {
  /// \ingroup memory_group
  static constexpr size_t record_alignment = 16;
  /// \ingroup memory_group
  static constexpr size_t record_mask = static_cast<size_t>(~0xf);

  /// \ingroup memory_group
  BAD(hd,inline,const)
  bool is_aligned(BAD(noescape) const void * ptr, std::uintptr_t alignment) noexcept {
    auto iptr = reinterpret_cast<std::uintptr_t>(ptr);
    return !(iptr % alignment);
  }

  /// \ingroup memory_group
  template <class T, size_t Alignment = record_alignment>
  struct aligned_allocator {
    using pointer = T*;
    using const_pointer = T const *;
    using void_pointer = void *;
    using const_void_pointer = void const *;
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    static constexpr size_t alignment = Alignment;

    template <class U> struct rebind {
      using other = aligned_allocator<T, alignment>;
    };

    aligned_allocator() = default;

    template <class U>
    BAD(hd,inline,noalias)
    constexpr aligned_allocator(BAD(noescape) const aligned_allocator<U,Alignment> &) noexcept {}

    // TODO: remaining attributes
    // can't specify alloc_size(X,Y) because i can't specify the constant comes from the sizeof(T)
    BAD(nodiscard,hd,inline,malloc,assume_aligned(alignment),returns_nonnull)
    T * allocate(size_t n) const noexcept {
      assert(n < std::numeric_limits<std::size_t>::max() / sizeof(T));
      T* result = static_cast<T*>(aligned_alloc(alignment,n));
      assert(result);
      return result;
    }

    BAD(hd,inline,noalias)
    void deallocate(BAD(noescape) T *p, BAD(maybe_unused) size_t n) noexcept {
      std::free(p);
    }

    BAD(hd,inline,noalias)
    void deallocate(BAD(noescape) T *p) noexcept {
      std::free(p);
    }

    template <class U>
    BAD(hd,inline,const)
    friend bool operator ==(
      BAD(maybe_unused) aligned_allocator<T,Alignment>,
      BAD(maybe_unused) aligned_allocator<U,Alignment>
    ) noexcept {
      return true;
    }

    template <class U>
    BAD(hd,inline,const)
    friend bool operator !=(
      BAD(maybe_unused) aligned_allocator<T,Alignment>,
      BAD(maybe_unused) aligned_allocator<U,Alignment>
    ) noexcept {
      return false;
    }
  };

  /// \ingroup memory_group
  using default_allocator = aligned_allocator<std::byte, record_alignment>;
}

namespace bad {
  using namespace bad::memory;
}

#endif
