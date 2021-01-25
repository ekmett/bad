#pragma once

#include "attributes.hh"

/// @file operators.hh
/// @brief operators and move semantics
/// @author Edward Kmett
/// @author Daniel Friy

/// @defgroup operators operators
/// @brief operators and move semantics

/// @{

namespace bad {
  ///  operators and move semantics
  namespace operators {
    /// re-exported by \ref bad and \ref bad::operators::api
    namespace common {}
    /// public components
    namespace api {
      using namespace common;
    }
    using namespace api;
  }
  using namespace operators::common;
}

#define bad_op(name,op) \
  /** provides `T op U` given `T op## = U`. @n\
   @param T current type @n\
   @param U the other type */\
  template<class T, class U = T>\
  struct BAD(empty_bases) name {\
    BAD(hd,nodiscard,inline,flatten)\
    friend T operator op(T const & lhs, T const & rhs)\
    noexcept(noexcept(T(lhs), std::declval<T&>() op## = rhs,T(std::declval<T&>()))) {\
      T x(lhs); x op## = rhs; return x;\
    }\
    BAD(hd,nodiscard,inline,flatten)\
    friend T operator op(T const & lhs, T && rhs)\
    noexcept(noexcept(T(lhs), std::declval<T&>() op## = std::move(rhs),T(std::declval<T&>()))) {\
      T x(lhs); x op## = std::move(rhs); return x;\
    }\
    BAD(hd,nodiscard,inline,flatten)\
    friend T operator op(T && lhs, T const & rhs)\
    noexcept(noexcept(T(std::move(lhs)), std::declval<T&>() op## = rhs, T(std::declval<T&>()))) {\
      T x(std::move(lhs)); x op## = rhs; return x;\
    }\
    BAD(hd,nodiscard,inline,flatten)\
    friend T operator op(T && lhs, T && rhs)\
    noexcept(noexcept(T(std::move(lhs)), std::declval<T&>() op## = std::move(rhs), T(std::declval<T&>()))) {\
      T x(std::move(lhs)); x op## = std::move(rhs); return rhs;\
    }\
  }

#define bad_op_left(name,op) \
  bad_op(name,op);\
  /** provides `U op T` given both `T(U)` and `T op## = U` @n\
   @param T current type @n\
   @param U the other type */\
  template<class T, class U>\
  struct BAD(empty_bases) name##_left {\
    BAD(hd,nodiscard,inline,flatten)\
    friend T operator op(U const & lhs, T const & rhs)\
    noexcept(noexcept(T(lhs), std::declval<T&>() op## = rhs, T(std::declval<T&>()))) {\
      T x(lhs); x op## = rhs; return x;\
    }\
    BAD(hd,nodiscard,inline,flatten)\
    friend T operator op(U const & lhs, T&& rhs)\
    noexcept(noexcept(T(lhs), std::declval<T&>() op## = std::move(rhs), T(std::declval<T&>()))) {\
      T x(lhs); x op## = std::move( rhs ); return x;\
    }\
    BAD(hd,nodiscard,inline,flatten)\
    friend T operator op(U&& lhs, T const & rhs)\
    noexcept(noexcept(T(std::move(lhs)), std::declval<T&>() op## = rhs, T(std::declval<T&>()))) {\
      T x(std::move(lhs)); x op## = rhs; return x;\
    }\
    friend T operator op(U&& lhs, T&& rhs)\
    noexcept(noexcept(T(std::move(lhs)), std::declval<T&>() op## = std::move(rhs), T(std::declval<T&>()))) {\
      T x( std::move(lhs)); x op## = std::move( rhs ); return x;\
    }\
  }

#define bad_op_commutative(name,op) \
  bad_op_left(name,op);\
  /** provides `T op U` and `U op T` given `T op## = U` @n\
   @param T current type @n\
   @param U the other type */\
  template<class T, class U = T >\
  struct BAD(empty_bases) commutative_##name {\
    BAD(hd,nodiscard,inline,flatten)\
    friend T operator op(T const & lhs, U const & rhs )\
    noexcept(noexcept(T(lhs), std::declval<T&>() op## = rhs,T(std::declval<T&>()))) {\
      T x(lhs); x op## = rhs; return x;\
    }\
    BAD(hd,nodiscard,inline,flatten)\
    friend T operator op(T const & lhs, U&& rhs )\
    noexcept(noexcept(T(lhs), std::declval<T&>() op## = std::move(rhs), T(std::declval<T&>()))) {\
      T x(lhs); x op## = std::move(rhs); return x;\
    }\
    BAD(hd,nodiscard,inline,flatten)\
    friend T operator op(T&& lhs, U const & rhs )\
    noexcept(noexcept(T(std::move(lhs)), std::declval<T&>() op## = rhs, T(std::declval<T&>()))) {\
      T x(std::move(lhs)); x op## = rhs; return x;\
    }\
    BAD(hd,nodiscard,inline,flatten)\
    friend T operator op(T&& lhs, U&& rhs)\
    noexcept(noexcept(T(std::move(lhs)), std::declval<T&>() op## = std::move(rhs), T(std::declval<T&>()))) {\
      T x(std::move(lhs)); x op## = std::move(rhs); return x;\
    }\
    BAD(hd,nodiscard,inline,flatten)\
    friend T operator op( const U& lhs, const T& rhs )\
    noexcept(noexcept(T(rhs), std::declval<T&>() op## = lhs, T(std::declval<T&>()))) {\
      T x(rhs); x op## = lhs; return x;\
    }\
    BAD(hd,nodiscard,inline,flatten)\
    friend T operator op(U const & lhs, T&& rhs)\
    noexcept(noexcept(T(std::move(rhs)),std::declval<T&>() op## = lhs, T(std::declval<T&>()))) {\
      T x( std::move( rhs ) ); x op## = lhs; return x;\
    }\
    BAD(hd,nodiscard,inline,flatten)\
    friend T operator op(U&& lhs, T const & rhs)\
    noexcept(noexcept(T(rhs), std::declval<T&>() op## = std::move(lhs))) {\
      T x( rhs ); x op## = std::move( lhs ); return x;\
    }\
    BAD(hd,nodiscard,inline,flatten)\
    friend T operator op(U&& lhs, T&& rhs)\
    noexcept(noexcept(T(std::move(rhs)), std::declval<T&>() op## = std::move(lhs))) {\
      T x(std::move(rhs)); x op## = std::move(lhs); return x;\
    }\
  };\
\
  template< typename T >\
  /** provides `T op T` given `T op##= T` @n\
   @param T current type */\
  struct BAD(empty_bases) commutative_##name< T > {\
    BAD(hd,nodiscard,inline,flatten)\
    friend T operator op(T const & lhs, T const & rhs)\
    noexcept(noexcept(T(lhs), std::declval<T&>() op## = rhs, T(std::declval<T&>()))) {\
      T x(lhs); x op## = rhs; return x;\
    }\
    BAD(hd,nodiscard,inline,flatten)\
    friend T operator op(T const & lhs, T&& rhs)\
    noexcept(noexcept(T(lhs), std::declval< T& >() op## = std::move(rhs), T(std::declval<T&>()))) {\
      T x(lhs); x op## = std::move( rhs ); return x;\
    }\
    BAD(hd,nodiscard,inline,flatten)\
    friend T operator op(T&& lhs, T const & rhs)\
    noexcept(noexcept(T(std::move(lhs)), std::declval<T&>() op## = rhs, T(std::declval<T&>()))) {\
      T x(std::move(lhs)); x op## = rhs; return x;\
    }\
    BAD(hd,nodiscard,inline,flatten)\
    friend T operator op(T&& lhs, T&& rhs)\
    noexcept(noexcept(T(std::move(lhs)), std::declval<T&>() op## = std::move(rhs), T(std::declval<T&>()))) {\
      T x(std::move(lhs));\
      x op## = std::move(rhs);\
      return x;\
    }\
  }

namespace bad::operators::api {

  /// @defgroup comparison comparison
  /// @ingroup operators
  /// @{

  /// provides `T != U`, `U == T`, and `U != T`, when given `T == U`
  /// @param T current type
  /// @param U the other type
  template <class T, class U=T>
  struct BAD(empty_bases) equality_comparable {
    BAD(hd,nodiscard,inline,flatten) 
    friend constexpr bool operator != (T const & lhs,U const & rhs)
    noexcept(noexcept(static_cast<bool>(lhs==rhs))) {
      return !static_cast<bool>(lhs==rhs);
    }

    BAD(hd,nodiscard,inline,flatten) 
    friend constexpr bool operator == (U const & lhs,T const & rhs)
    noexcept(noexcept(static_cast<bool>(lhs==rhs))) {
      return static_cast<bool>(lhs==rhs);
    }

    BAD(hd,nodiscard,inline,flatten) 
    friend constexpr bool operator != (U const & lhs,T const & rhs)
    noexcept(noexcept(static_cast<bool>(lhs!=rhs))) {
      return static_cast<bool>(lhs!=rhs);
    }
  };

  /// provides `T != T` when given `T == T`
  /// @param T current type
  template <class T>
  struct BAD(empty_bases) equality_comparable<T> {
    BAD(hd,nodiscard,inline,flatten) 
    friend constexpr bool operator != (T const & lhs,T const & rhs)
    noexcept(noexcept(static_cast<bool>(lhs==rhs))) {
      return !static_cast<bool>(lhs==rhs);
    }
  };

  /// @}

  /// @defgroup arithmetic arithmetic
  /// @ingroup operators
  /// @{

  bad_op_commutative(addable, +);
  bad_op_left(subtractable, -);
  bad_op_commutative(multipliable, *);
  bad_op_left(dividable, /);
  bad_op_left(modable, % );

  /// provides `T @ U` and `U @ T` when given `T(U)`, `T @= U` for `@` in `{ +, -, * }`
  /// @param T current type
  /// @param U the other type
  template <class T, class U=T>
  struct BAD(empty_bases) ringlike
  : commutative_addable<T,U>
  , subtractable<T,U>
  , subtractable_left<T,U>
  , multipliable<T,U> {};

  /// @}

  /// @defgroup bitwise bitwise
  /// @ingroup operators
  /// @{

  bad_op_commutative(andable, &);
  bad_op_commutative(orable, |);
  bad_op_commutative(xorable, ^);

  /// provides `T @ U` when given `T @= U` for `@` in `{ &, |, ^ }`
  /// @param T current type
  /// @param U the other type
  template <class T, class U=T>
  struct BAD(empty_bases) bitwise
  : andable<T,U>
  , orable<T,U>
  , xorable<T,U> {};

  /// provides `U @ T` when given `T @= U` and `T(U)` for `@` in `{ &, |, ^ }`
  /// @param T current type
  /// @param U the other type
  template <class T, class U=T>
  struct BAD(empty_bases) bitwise_left
  : andable_left<T,U>
  , orable_left<T,U>
  , xorable_left<T,U> {};

  /// provides `T @ U` and `U @ T` when given `T @= U` in `{ &, |, ^ }`
  /// @param T current type
  /// @param U the other type
  template <class T, class U=T>
  struct BAD(empty_bases) commutative_bitwise
  : commutative_andable<T,U>
  , commutative_orable<T,U>
  , commutative_xorable<T,U> {};

  bad_op(left_shiftable, <<);
  bad_op(right_shiftable, >>);

  template <class T, class U=T>
  struct BAD(empty_bases) shiftable
  : left_shiftable<T,U>
  , right_shiftable<T,U> {};

  /// @}
}

#undef bad_op
#undef bad_op_left
#undef bad_op_commutative

/// @}

// The MIT License (MIT)
// 
// Copyright (c) 2021 Edward Kmett
// Copyright (c) 2013-2020 Daniel Frey
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
