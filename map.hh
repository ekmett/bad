#pragma once

#define BAD_EVAL0(...) __VA_ARGS__
#define BAD_EVAL1(...) BAD_EVAL0(BAD_EVAL0(BAD_EVAL0(__VA_ARGS__)))
#define BAD_EVAL2(...) BAD_EVAL1(BAD_EVAL1(BAD_EVAL1(__VA_ARGS__)))
#define BAD_EVAL3(...) BAD_EVAL2(BAD_EVAL2(BAD_EVAL2(__VA_ARGS__)))
#define BAD_EVAL4(...) BAD_EVAL3(BAD_EVAL3(BAD_EVAL3(__VA_ARGS__)))
#define BAD_EVAL(...)  BAD_EVAL4(BAD_EVAL4(BAD_EVAL4(__VA_ARGS__)))

#define BAD_MAP_END(...)
#define BAD_MAP_OUT
#define BAD_MAP_COMMA ,

#define BAD_MAP_GET_END2() 0, BAD_MAP_END
#define BAD_MAP_GET_END1(...) BAD_MAP_GET_END2
#define BAD_MAP_GET_END(...) BAD_MAP_GET_END1
#define BAD_MAP_NEXT0(test, next, ...) next BAD_MAP_OUT
#define BAD_MAP_NEXT1(test, next) BAD_MAP_NEXT0(test, next, 0)
#define BAD_MAP_NEXT(test, next)  BAD_MAP_NEXT1(BAD_MAP_GET_END test, next)

#define BAD_MAP0(f, x, peek, ...) f(x) BAD_MAP_NEXT(peek, BAD_MAP1)(f, peek, __VA_ARGS__)
#define BAD_MAP1(f, x, peek, ...) f(x) BAD_MAP_NEXT(peek, BAD_MAP0)(f, peek, __VA_ARGS__)

#define BAD_MAP_LIST_NEXT1(test, next) BAD_MAP_NEXT0(test, BAD_MAP_COMMA next, 0)
#define BAD_MAP_LIST_NEXT(test, next)  BAD_MAP_LIST_NEXT1(BAD_MAP_GET_END test, next)

#define BAD_MAP_LIST0(f, x, peek, ...) f(x) BAD_MAP_LIST_NEXT(peek, BAD_MAP_LIST1)(f, peek, __VA_ARGS__)
#define BAD_MAP_LIST1(f, x, peek, ...) f(x) BAD_MAP_LIST_NEXT(peek, BAD_MAP_LIST0)(f, peek, __VA_ARGS__)

/// Applies the function macro `f` to each of the parameters.
#define BAD_MAP(f, ...) BAD_EVAL(BAD_MAP1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))


/// Applies the function macro `f` to each of the parameters. Inserts commas between the results.
#define BAD_MAP_LIST(f, ...) BAD_EVAL(BAD_MAP_LIST1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

/*
 * Copyright (C) 2012 William Swanson
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
