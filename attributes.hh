#pragma once
#include "macros.hh"

/// @file attributes.hh
/// macros for applying attributes.
///
/// @defgroup attributes attributes
/// macros for applying attributes to functions, members, arguments, etc.
/// @{
///
/// @def BAD(...)
/// @brief convenient macro for applying several attributes
///
/// e.g. BAD(hd,inline) vs. \ref bad_hd \ref bad_inline
///
/// erased for doxygen. so _only_ use this for attributes

#ifndef DOXYGEN
#define BAD(...) BAD_MAP(bad_,__VA_ARGS__)
#else
#define BAD(...)
#endif

/// @def bad_has_attribute(__x)
/// @brief portable `__has_attribute(x)`
#ifdef __has_attribute
#define bad_has_attribute(__x) __has_attribute(__x)
#else
#define bad_has_attribute(__x) 0
#endif

/// @def bad_has_declspec_attribute(__x)
/// @brief portable `__has_declspec_attribute(__x)`
#if defined(_MSC_VER)
#define bad_has_declspec_attribute(__x) 1
#elif defined(BAD_USE_DECLSPEC) && defined(__has_declspec_attribute)
#define bad_has_declspec_attribute(__x) __has_declspec_attribute(__x)
#else
#define bad_has_declspec_attribute(__x) 0
#endif

/// @def bad_has_cpp_attribute(x)
/// @brief portable `__has_cpp_attribute(x)`
#ifdef __has_cpp_attribute
#define bad_has_cpp_attribute(x) __has_cpp_attribute(x)
#else
#define bad_has_cpp_attribute(x) 0
#endif

/// @def bad_likely(x)
/// @brief portable `[[likely]]` or `__builtin_expect(x,1)` expressions

/// @def bad_unlikely(x)
/// @brief portable `[[unlikely]]` or `__builtin_expect(x,0)` expressions
#if bad_has_cpp_attribute(likely)
#define bad_likely(x) (x) [[likely]]
#define bad_unlikely(x) (x) [[unlikely]]
#else
#define bad_likely(x) __builtin_expect(!!(x),1)
#define bad_unlikely(x) __builtin_expect(!!(x),0)
#endif

// TODO: restrict
// TODO: gsl::Owner, gsl::Pointer?

/// @def bad_inline
/// @brief portable `inline __forceinline` or `inline __attribute__((always_inline))`

#if bad_has_attribute(always_inline)
#define bad_inline inline __attribute__((always_inline))
#elif defined _WIN32
#define bad_inline __forceinline
#else
#define bad_inline inline
#endif

/// @def bad_preferred_name(x)
/// @brief portable `[[clang::preferred_name(x)]]` annotations
///
/// can be applied to a template struct to provide better names
/// for some concrete instantiations

#if bad_has_cpp_attribute(clang::preferred_name)
#define bad_preferred_name(__x) [[clang::preferred_name(__x)]]
#else
#define bad_preferred_name(__x)
#endif

/// @def bad_flatten
/// @brief portable `[[flatten]]`
///
/// the compiler should inline recursively aggressively under
/// this definition. use with care, good for expression templates

#if bad_has_attribute(flatten)
#define bad_flatten __attribute__((flatten))
#else
#define bad_flatten
#warning no flatten
#endif

/// @def bad_malloc
/// @brief portable `__attribute__((malloc))` attribute
///
/// indicates the returned memory does not alias with any other pointer

#if bad_has_attribute(malloc)
#define bad_malloc __attribute__((malloc))
#else
#define bad_malloc
#warning no malloc
#endif

/// @def bad_uninitialized
/// @brief portable `[[clang::uninitialized]]` attribute
///
/// ensures a stack variable remains uninitialized regardless of
/// `-ftrivial-auto-var-init=*` settings passed to the compiler

#if bad_has_attribute(uninitialized)
#define bad_uninitialized __attribute__((uninitialized))
#else
#define bad_uninitialized
#warning no uninitialized
#endif

/// @def bad_noalias
/// @brief portable `__declspec(noalias)`
///
/// indicates a function only accesses data through pointer arguments
///
/// requires clang to be running with `-fdeclspec` or `-fms-extensions`
/// but can aid alias analysis
#if bad_has_declspec_attribute(noalias)
#define bad_noalias __declspec(noalias)
#else
#define bad_noalias
#warning no noalias
#endif

/// @def bad_require_constant_initialization
/// @brief portable `constinit`
///
/// `constinit` requires C++20, this is available earlier
#if bad_has_attribute(require_constant_initialization)
#define bad_constinit __attribute__((require_constant_initialization))
#else
#define bad_constinit
#warning no constinit
#endif

//#if bad_has_attribute(called_once)
//#define BAD_CALLED_ONCE __attribute__((called_once))
//#else
//#define BAD_CALLED_ONCE
//#warning no called_once
//#endif

/// @def bad_const
/// @brief portable `__attribute__((const))`. NOT `const`
///
/// indicates the result is entirely determined by the arguments
/// and does not access main memory in any way, including accessing
/// members of this
///
/// allows the compiler to easily elide/duplicate calls
#if bad_has_attribute(const)
#define bad_const __attribute__((const))
#else
#define bad_const
#warning no const
#endif

/// @def bad_pure
/// @brief portable `__attribute__((pure))`
///
/// no side-effects other than return value, may inspect globals
///
/// allows the compiler to easily elide/duplicate calls
#if bad_has_attribute(pure)
#define bad_pure __attribute__((pure))
#else
#define bad_pure
#warning no pure
#endif

/// @def bad_reinitializes
/// @brief portable `[[clang::reinitializes]]`
///
/// indicates to any uninitialised object state sanitizer
/// that this restores an object to a fresh state independent
/// of its previous state
#if __has_cpp_attribute(clang::reinitializes)
#define bad_reinitializes [[clang::reinitializes]]
#else
#define bad_reinitializes
#warning no reinitializes
#endif

/// @def bad_assume_aligned(x)
/// @brief portable `__attribute__((assume_aligned(x)))`
///
/// function returns a pointer with alignment that is at least x

#if bad_has_attribute(assume_aligned)
#define bad_assume_aligned(x) __attribute__((assume_aligned(x)))
#else
#define bad_assume_aligned(x)
#warning no assume_aligned
#endif

/// @def bad_align_value(x)
/// @brief portable `__attribute__((align_value(x)))`
///
/// the annotated pointer specified has alignment at least x

#if bad_has_attribute(align_value)
#define bad_align_value(x) __attribute__((align_value(x)))
#else
#define bad_align_value(x)
#warning no align_value
#endif

/// @def bad_alloc_align(x)
/// @brief portable `__attribute__((alloc_align(x)))`
///
/// Indicates the 1-based argument number of a function that indicates the alignment of the returned result
#if bad_has_attribute(alloc_align)
#define bad_alloc_align(x) __attribute__((alloc_align(x)))
#else
#define bad_alloc_align(x)
#warning no alloc_align
#endif

/// @def bad_alloc_size(x)
/// @brief portable `__attribute__((alloc_size(x)))`
///
/// arg # (1-based) of the attribute that tells you the size of the result in bytes
#if bad_has_attribute(alloc_size)
#define bad_alloc_size(x) __attribute__((alloc_size(x)))
#else
#define bad_alloc_size(x)
#warning no alloc_size
#endif

/// @def bad_noescape
/// @brief portable `__attribute__((noescape))`
///
/// argument is not captured by the function (rust-style borrow)

#if bad_has_attribute(noescape)
#define bad_noescape __attribute__((noescape))
#else
#define bad_noescape
#warning no noescape
#endif

/// @def bad_callback(...)
/// @brief portable `__attribute__((callback(...)))`
///
/// indicates the specified argument will be called back with the
/// other named arguments. complicated, see clang docs.
///
/// allows better interprocedural analysis

#if bad_has_attribute(callback)
#define bad_callback(...) __attribute__((callback(__VA_ARGS__)))
#else
#define bad_callback(...)
#warning no callback
#endif

/// @def bad_lifetimebound
/// @brief portable `__attribute__((lifetimebound))`
///
/// the argument must be kept alive as long as the result of the
/// function is alive. Should be used for functions that return
/// references or views into the target object.
///
/// Sadly, clang (as of Jan 2021) does not yet allow it to be
/// applied to the implicit this argument of an member
/// function

#if bad_has_attribute(lifetimebound)
#define bad_lifetimebound __attribute__((lifetimebound))
#else
#define bad_lifetimebound
#warning no lifetimebound
#endif

/// @def bad_returns_nonnull
/// @brief portable `__attribute__((returns_nonnnull))`
///
/// The result of this function will be non-null

#if bad_has_attribute(returns_nonnull)
#define bad_returns_nonnull __attribute__((returns_nonnull))
#else
#define bad_returns_nonnull
#warning no returns_nonnull
#endif

/// @def bad_host
/// @brief portable `__host__` for cuda
//
/// indicates the function should be available on the host
//
/// @def bad_device
/// @brief portable `__device__` for cuda
///
/// indicates the function should be available on the device
//
/// @def bad_global
/// @brief portable `__global__` for cuda
///
/// indicates the function is a global entry point for a 
/// compute kernel

#ifdef __CUDACC__
#define bad_host __host__
#define bad_device __device__
#define bad_global __global__
#else
#define bad_host
#define bad_device
#define bad_global
#endif

/// @def bad_hd
/// @brief applies both \ref bad_host and \ref bad_device
///
/// most functions in the library should be defined this way
///
/// may eventually add similar annotations for sycl and the like

#define bad_hd bad_host bad_device

/// @def bad_no_unique_address
/// @brief portable c++20 `[[no_unique_address]]`
///
/// clang makes this available slightly earlier

#if bad_has_cpp_attribute(no_unique_address)
#define bad_no_unique_address [[no_unique_address]]
#else
#define bad_no_unique_address
#endif

/// @def bad_maybe_unused
/// @brief argument, member or definition `[[maybe_unused]]`
///
/// @since C++17
#define bad_maybe_unused [[maybe_unused]]

/// @def bad_deprecated
/// @brief `[[deprecated]]`
///
/// @since C++14
#define bad_deprecated [[deprecated]]

/// @def bad_nodiscard
/// @brief C++17 `[[nodiscard]]`.
/// The user should explicitly throw away the result rather than let it be silently discarded
#define bad_nodiscard [[nodiscard]]

/// @}
