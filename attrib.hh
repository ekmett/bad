#pragma once
#include "map.hh"

#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

#if __has_attribute(always_inline)
#define BAD_INLINE inline __attribute__ ((always_inline))
#elif defined _WIN32
#define BAD_INLINE __forceinline
#else
#define BAD_INLINE inline
#endif

#if __has_attribute(flatten)
#define BAD_FLATTEN __attribute__((flatten))
#else
#define BAD_FLATTEN
#endif

#if __has_attribute(malloc)
#define BAD_MALLOC __attribute__((malloc))
#else
#define BAD_MALLOC
#endif

/// only modifies data reachable through pointer arguments
#if defined(__clang__) || defined(_MSC_VER)
#define BAD_NOALIAS __declspec(noalias)
#else
#define BAD_NOALIAS
#endif

/// only examines arguments, no effect other than return value
#if __has_attribute(const)
#define BAD_CONST __attribute__((const))
#else
#define BAD_CONST
#endif

/// no effect other than return value, may inspect globals
#if __has_attribute(pure)
#define BAD_PURE __attribute__((pure))
#else
#define BAD_PURE
#endif

#if __has_attribute(deprecated)
#define BAD_DEPRECATED __attribute__((deprecated))
#elif defined _MSC_VER
#define BAD_DEPRECATED __declspec(deprecated)
#else
#define BAD_DEPRECATED
#endif

#if __has_attribute(assume_aligned)
#define BAD_ASSUME_ALIGNED(x) __attribute__((assume_aligned(x)))
#else
#define BAD_ASSUME_ALIGNED(x)
#endif

// arg # (1-based) of the attribute that tells you the alignment of the result

#if __has_attribute(align_value)
#define BAD_ALIGN_VALUE(x) __attribute__((align_value(x)))
#else
#define BAD_ALIGN_VALUE(x)
#endif

// arg # (1-based) of the attribute that tells you the alignment of the result
#if __has_attribute(alloc_align)
#define BAD_ALLOC_ALIGN(x) __attribute__((alloc_align(x)))
#else
#define BAD_ALLOC_ALIGN(x)
#endif

// arg # (1-based) of the attribute that tells you the size of the result in bytes
#if __has_attribute(alloc_size)
#define BAD_ALLOC_SIZE(x) __attribute__((alloc_size(x)))
#else
#define BAD_ALLOC_SIZE(x)
#endif

#if __has_attribute(returns_nonnull)
#define BAD_RETURNS_NONNULL __attribute__((returns_nonnull))
#else
#define BAD_RETURNS_NONNULL
#endif

#ifdef __CUDACC__
#define BAD_HOST __host__
#define BAD_DEVICE __device__
#define BAD_GLOBAL __global__
#else
#define BAD_HOST
#define BAD_DEVICE
#define BAD_GLOBAL
#endif

#define BAD_HD BAD_HOST BAD_DEVICE

/// use the c++ standard attribute
#define BAD_MAYBE_UNUSED [[maybe_unused]]

