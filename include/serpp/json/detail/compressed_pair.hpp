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

#ifndef SERPP_COMPRESSED_PAIR_HPP
#define SERPP_COMPRESSED_PAIR_HPP

#include <type_traits>
#include <utility>

namespace serpp::json::detail {
struct zero_then_variadic_args {
    explicit zero_then_variadic_args() = default;
};

struct one_then_variadic_args {
    explicit one_then_variadic_args() = default;
};

template<typename Empty, typename Value>
    requires(std::is_empty_v<Empty>)
struct compressed_pair : private Empty {
    template<typename... Args>
    compressed_pair(zero_then_variadic_args, Args&&... args) noexcept(std::is_nothrow_default_constructible_v<Empty> &&
                                                                      std::is_nothrow_constructible_v<Value, Args...>)
        requires(std::is_default_constructible_v<Empty> && std::is_constructible_v<Value, Args...>)
        : Empty{}
        , second{std::forward<Args>(args)...} {}

    template<typename T, typename... Args>
    compressed_pair(one_then_variadic_args, T&& first, Args&&... args) noexcept
        : Empty{std::move(first)}
        , second{std::forward<Args>(args)...} {}

    auto first() noexcept -> Empty& {
        return *this;
    }

    auto first() const noexcept -> Empty& {
        return *this;
    }

    Value second;
};
}// namespace serpp::json::detail

#endif// #ifndef SERPP_COMPRESSED_PAIR_HPP
