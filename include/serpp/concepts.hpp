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

#ifndef SERPP_CONCEPTS_HPP
#define SERPP_CONCEPTS_HPP

#include <concepts>
#include <iterator>
#include <type_traits>

namespace serpp {
template<typename T, template<typename...> typename Template>
inline constexpr bool is_specialisation_v = false;

template<template<typename...> typename Template, typename... Ts>
inline constexpr bool is_specialisation_v<Template<Ts...>, Template> = true;

template<typename T, template<typename...> typename Template>
concept is_specialisation_of = is_specialisation_v<T, Template>;

template<typename T, template<typename...> typename Template>
concept is_not_specialisation_of = !is_specialisation_v<T, Template>;

template<typename T, typename U>
concept equality_comparable_with = requires {
    { std::declval<T>() == std::declval<U>() } -> std::same_as<bool>;
    { std::declval<T>() != std::declval<U>() } -> std::same_as<bool>;
};

template<typename T, typename U>
concept order_comparable_with = requires {
    { std::declval<T>() < std::declval<U>() } -> std::same_as<bool>;
    { std::declval<T>() <= std::declval<U>() } -> std::same_as<bool>;
    { std::declval<T>() > std::declval<U>() } -> std::same_as<bool>;
    { std::declval<T>() >= std::declval<U>() } -> std::same_as<bool>;
};
}// namespace serpp

#endif// #ifndef SERPP_CONCEPTS_HPP
