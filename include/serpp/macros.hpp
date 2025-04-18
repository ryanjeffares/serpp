/*
 * Copyright © 2025 Ryan Jeffares
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the “Software”), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef SERPP_MACROS_HPP
#define SERPP_MACROS_HPP

#ifndef SERPP_DECLARE_DEFAULT_COPYABLE
#define SERPP_DECLARE_DEFAULT_COPYABLE(type)        \
    type(const type&) = default;                    \
    auto operator=(const type&) -> type& = default;
#endif// #ifndef SERPP_DECLARE_DEFAULT_COPYABLE

#ifndef SERPP_DECLARE_DEFAULT_MOVABLE
#define SERPP_DECLARE_DEFAULT_MOVABLE(type)             \
    type(type&&) noexcept = default;                    \
    auto operator=(type&&) noexcept -> type& = default;
#endif// #ifndef SERPP_DECLARE_DEFAULT_MOVABLE

#ifndef SERPP_DECLARE_DEFAULT_COPYABLE_MOVABLE
#define SERPP_DECLARE_DEFAULT_COPYABLE_MOVABLE(type) \
    SERPP_DECLARE_DEFAULT_COPYABLE(type)             \
    SERPP_DECLARE_DEFAULT_MOVABLE(type)
#endif// #ifndef SERPP_DECLARE_DEFAULT_COPYABLE_MOVABLE

#ifndef SERPP_DECLARE_NON_COPYABLE
#define SERPP_DECLARE_NON_COPYABLE(type)            \
    type(const type&) = delete;                     \
    auto operator=(const type&) -> type& = delete;
#endif// #ifndef SERPP_DECLARE_NON_COPYABLE

#ifndef SERPP_DECLARE_NON_MOVABLE
#define SERPP_DECLARE_NON_MOVABLE(type)                 \
    type(type&&) noexcept = delete;                     \
    auto operator=(type&&) noexcept -> type& = delete;
#endif// #ifndef SERPP_DECLARE_NON_MOVABLE

#ifndef SERPP_DECLARE_NON_COPYABLE_MOVABLE
#define SERPP_DECLARE_NON_COPYABLE_MOVABLE(type)    \
    SERPP_DECLARE_NON_COPYABLE(type)                \
    SERPP_DECLARE_NON_MOVABLE(type)
#endif// #ifndef SERPP_DECLARE_NON_COPYABLE_MOVABLE

#endif// #ifndef SERPP_MACROS_HPP
