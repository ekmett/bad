#pragma once

#define BAD_EVAL0_(...) __VA_ARGS__
#define BAD_EVAL1_(...) BAD_EVAL0_(BAD_EVAL0_(BAD_EVAL0_(__VA_ARGS__)))
#define BAD_EVAL2_(...) BAD_EVAL1_(BAD_EVAL1_(BAD_EVAL1_(__VA_ARGS__)))
#define BAD_EVAL3_(...) BAD_EVAL2_(BAD_EVAL2_(BAD_EVAL2_(__VA_ARGS__)))
#define BAD_EVAL4_(...) BAD_EVAL3_(BAD_EVAL3_(BAD_EVAL3_(__VA_ARGS__)))
#define BAD_EVAL_(...)  BAD_EVAL4_(BAD_EVAL4_(BAD_EVAL4_(__VA_ARGS__)))

#define BAD_MAP_END_(...)
#define BAD_MAP_OUT_
#define BAD_MAP_COMMA_ ,

#define BAD_MAP_GET_END2_() 0, BAD_MAP_END_
#define BAD_MAP_GET_END1_(...) BAD_MAP_GET_END2_
#define BAD_MAP_GET_END_(...) BAD_MAP_GET_END1_
#define BAD_MAP_NEXT0_(test, next, ...) next BAD_MAP_OUT_
#define BAD_MAP_NEXT1_(test, next) BAD_MAP_NEXT0_(test, next, 0)
#define BAD_MAP_NEXT_(test, next)  BAD_MAP_NEXT1_(BAD_MAP_GET_END_ test, next)

#define BAD_MAP0_(f, x, peek, ...) f(x) BAD_MAP_NEXT_(peek, BAD_MAP1_)(f, peek, __VA_ARGS__)
#define BAD_MAP1_(f, x, peek, ...) f(x) BAD_MAP_NEXT_(peek, BAD_MAP0_)(f, peek, __VA_ARGS__)

#define BAD_MAP_LIST_NEXT1_(test, next) BAD_MAP_NEXT0_(test, BAD_MAP_COMMA_ next, 0)
#define BAD_MAP_LIST_NEXT_(test, next)  BAD_MAP_LIST_NEXT1_(BAD_MAP_GET_END_ test, next)

#define BAD_MAP_LIST0_(f, x, peek, ...) f(x) BAD_MAP_LIST_NEXT_(peek, BAD_MAP_LIST1_)(f, peek, __VA_ARGS__)
#define BAD_MAP_LIST1_(f, x, peek, ...) f(x) BAD_MAP_LIST_NEXT_(peek, BAD_MAP_LIST0_)(f, peek, __VA_ARGS__)

#define BAD_JOIN_(X,Y) X ## Y
#define bad_(Y) BAD_JOIN_(bad_,Y)

/// Applies the function macro `f` to each of the parameters.
#define BAD_MAP(f, ...) BAD_EVAL_(BAD_MAP1_(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

/// Applies the function macro `f` to each of the parameters. Inserts commas between the results.
#define BAD_MAP_LIST(f, ...) BAD_EVAL_(BAD_MAP_LIST1_(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

/// convenient macro for applying several attributes
// e.g. BAD(HD,INLINE) vs. BAD_HD BAD_INLINE
#define BAD(...) BAD_MAP(bad_,__VA_ARGS__)

/*
 * Copyright (C) 2012 William Swanson, 2021 Edward Kmett
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the names of the authors or
 * their institutions shall not be used in advertising or otherwise to
 * promote the sale, use or other dealings in this Software without
 * prior written authorization from the authors.
 */
