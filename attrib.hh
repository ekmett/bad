#pragma once
#include "map.hh"

#ifdef __has_attribute
#define bad_has_attribute(__x) __has_attribute(__x)
#else
#define bad_has_attribute(__x) 0
#endif

#if defined(_MSC_VER)
#define bad_has_declspec_attribute(__x) 1
#elif defined(BAD_USE_DECLSPEC) && defined(__has_declspec_attribute)
#define bad_has_declspec_attribute(__x) __has_declspec_attribute(__x)
#else
#define bad_has_declspec_attribute(__x) 0
#endif

#ifdef __has_cpp_attribute
#define bad_has_cpp_attribute(x) __has_cpp_attribute(x)
#else
#define bad_has_cpp_attribute(x) 0
#endif

#if bad_has_cpp_attribute(likely)
#define bad_likely(x) (x) [[likely]]
#define bad_unlikely(x) (x) [[unlikely]]
#else
#define bad_likely(x) __builtin_expect(!!(x),1)
#define bad_unlikely(x) __builtin_expect(!!(x),0)
#endif

// TODO: restrict
// TODO: gsl::Owner, gsl::Pointer?

#if bad_has_attribute(always_inline)
#define bad_inline inline __attribute__((always_inline))
#elif defined _WIN32
#define bad_inline __forceinline
#else
#define bad_inline inline
#endif

#if bad_has_attribute(flatten)
#define bad_flatten __attribute__((flatten))
#else
#define bad_flatten
#warning no flatten
#endif


#if bad_has_attribute(malloc)
#define bad_malloc __attribute__((malloc))
#else
#define bad_malloc
#warning no malloc
#endif


#if bad_has_attribute(uninitialized)
#define bad_uninitialized __attribute__((uninitialized))
#else
#define bad_uninitialized
#warning no uninitialized
#endif


/// only modifies data reachable through pointer arguments
#if bad_has_declspec_attribute(noalias)
#define bad_noalias __declspec(noalias)
#else
#define bad_noalias
#warning no noalias
#endif

/// only modifies data reachable through pointer arguments
#if bad_has_attribute(require_constant_initialization)
#define bad_constinit __attribute__((require_constant_initialization))
#else
#define bad_constinit
#warning no constinit
#endif

/// check that this is called once on all execution paths

/// check that this is called once on all execution paths
//#if bad_has_attribute(called_once)
//#define BAD_CALLED_ONCE __attribute__((called_once))
//#else
//#define BAD_CALLED_ONCE
//#warning no called_once
//#endif

/// no effect other than return value, may inspect globals

/// only examines arguments, no effect other than return value
#if bad_has_attribute(const)
#define bad_const __attribute__((const))
#else
#define bad_const
#warning no const
#endif

/// no effect other than return value, may inspect globals
#if bad_has_attribute(pure)
#define bad_pure __attribute__((pure))
#else
#define bad_pure
#warning no pure
#endif

#if __has_cpp_attribute(clang::reinitializes)
#define bad_reinitializes [[clang::reinitializes]]
#else
// TODO: support GCC
#define bad_reinitializes
#warning no reinitializes
#endif

// C++14

#if bad_has_attribute(assume_aligned)
#define bad_assume_aligned(x) __attribute__((assume_aligned(x)))
#else
#define bad_assume_aligned(x)
#warning no assume_aligned
#endif

// arg # (1-based) of the attribute that tells you the alignment of the result

#if bad_has_attribute(align_value)
#define bad_align_value(x) __attribute__((align_value(x)))
#else
#define bad_align_value(x)
#warning no align_value
#endif

// arg # (1-based) of the attribute that tells you the alignment of the result
#if bad_has_attribute(alloc_align)
#define bad_alloc_align(x) __attribute__((alloc_align(x)))
#else
#define bad_alloc_align(x)
#warning no alloc_align
#endif

// arg # (1-based) of the attribute that tells you the size of the result in bytes
#if bad_has_attribute(alloc_size)
#define bad_alloc_size(x) __attribute__((alloc_size(x)))
#else
#define bad_alloc_size(x)
#warning no alloc_size
#endif

#if bad_has_attribute(noescape)
#define bad_noescape __attribute__((noescape))
#else
#define bad_noescape
#warning no noescape
#endif

#if bad_has_attribute(callback)
#define bad_callback(...) __attribute__((callback(__VA_ARGS__)))
#else
#define bad_callback(...)
#warning no callback
#endif

#if bad_has_attribute(lifetimebound)
#define bad_lifetimebound __attribute__((lifetimebound))
#else
#define bad_lifetimebound
#warning no lifetimebound
#endif


#if bad_has_attribute(returns_nonnull)
#define bad_returns_nonnull __attribute__((returns_nonnull))
#else
#define bad_returns_nonnull
#warning no returns_nonnull
#endif

#ifdef __CUDACC__
#define bad_host __host__
#define bad_device __device__
#define bad_global __global__
#else
#define bad_host
#define bad_device
#define bad_global
#endif

#define bad_hd bad_host bad_device

/// @since C++20
#if bad_has_cpp_attribute(no_unique_address)
#define bad_no_unique_address [[no_unique_address]]
#else
#define bad_no_unique_address
#endif

/// @since C++17
#define bad_maybe_unused [[maybe_unused]]

/// @since C++14
#define bad_deprecated [[deprecated]]

/// @since C++17
#define bad_nodiscard [[nodiscard]]
