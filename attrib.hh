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

#if __has_attribute(deprecated)
#define BAD_DEPRECATED __attribute__((deprecated))
#elif defined _MSC_VER
#define BAD_DEPRECATED __declspec(deprecated)
#else
#define BAD_DEPRECATED
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

