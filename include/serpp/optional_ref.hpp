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

#ifndef SERPP_OPTIONAL_REF_HPP
#define SERPP_OPTIONAL_REF_HPP

#include <compare>
#include <concepts>

namespace serpp {
template<typename T>
class optional_ref {
public:
    optional_ref() = default;
    optional_ref(T& value) : m_ptr{&value} {}

    auto operator=(const optional_ref& other) noexcept -> optional_ref& {
        if (this != &other) { m_ptr = other.m_ptr; }

        return *this;
    }

    [[nodiscard]] auto has_value() const noexcept -> bool { return m_ptr != nullptr; }

    [[nodiscard]] auto get() const noexcept -> T& { return *m_ptr; }

    [[nodiscard]] operator T&() const noexcept { return get(); }

    [[nodiscard]] auto operator*() const noexcept -> T& { return get(); }

    [[nodiscard]] auto operator->() const noexcept -> T* { return m_ptr; }

private:
    T* m_ptr{nullptr};
};// class optional_ref

template<typename T, equality_comparable_with<T> U>
[[nodiscard]] auto operator==(const optional_ref<T>& l, const optional_ref<U>& r) noexcept -> bool {
    return !(l.has_value() != r.has_value()) && (l.has_value() == false || *l == *r);
}

template<typename T, equality_comparable_with<T> U>
[[nodiscard]] auto operator!=(const optional_ref<T>& l, const optional_ref<U>& r) noexcept -> bool {
    return !(l == r);
}

template<typename T, typename U>
    requires(order_comparable_with<T, U>)
[[nodiscard]] auto operator<(const optional_ref<T>& l, const optional_ref<U>& r) noexcept -> bool {
    return r.has_value() && (!l.has_value() || *l < *r);
}

template<typename T, typename U>
    requires(order_comparable_with<T, U>)
[[nodiscard]] auto operator<=(const optional_ref<T>& l, const optional_ref<U>& r) noexcept -> bool {
    return r.has_value() && (!l.has_value() || *l <= *r);
}

template<typename T, typename U>
    requires(order_comparable_with<T, U>)
[[nodiscard]] auto operator>(const optional_ref<T>& l, const optional_ref<U>& r) noexcept -> bool {
    return r.has_value() && (!l.has_value() || *l > *r);
}

template<typename T, typename U>
    requires(order_comparable_with<T, U>)
[[nodiscard]] auto operator>=(const optional_ref<T>& l, const optional_ref<U>& r) noexcept -> bool {
    return r.has_value() && (!l.has_value() || *l >= *r);
}

template<typename T, std::three_way_comparable_with<T> U>
[[nodiscard]] auto operator<=>(const optional_ref<T>& l, const optional_ref<U>& r) noexcept
    -> std::compare_three_way_result<T, U> {
    return l.has_value() & r.has_value() ? *l <=> *r : l.has_value() <=> r.has_value();
}

template<typename T, typename U>
    requires(equality_comparable_with<T, U> && is_not_specialisation_of<U, optional_ref>)
[[nodiscard]] auto operator==(const optional_ref<T>& opt, const U& value) noexcept -> bool {
    return opt.has_value() && *opt == value;
}

template<typename T, typename U>
    requires(equality_comparable_with<T, U> && is_not_specialisation_of<U, optional_ref>)
[[nodiscard]] auto operator==(const U& value, const optional_ref<T>& opt) noexcept -> bool {
    return opt.has_value() && value == *opt;
}

template<typename T, typename U>
    requires(equality_comparable_with<T, U> && is_not_specialisation_of<U, optional_ref>)
[[nodiscard]] auto operator!=(const optional_ref<T>& opt, const U& value) noexcept -> bool {
    return !(opt == value);
}

template<typename T, typename U>
    requires(equality_comparable_with<T, U> && is_not_specialisation_of<U, optional_ref>)
[[nodiscard]] auto operator!=(const U& value, const optional_ref<T>& opt) noexcept -> bool {
    return !(value == opt);
}

template<typename T, typename U>
    requires(order_comparable_with<T, U> && is_not_specialisation_of<U, optional_ref>)
[[nodiscard]] auto operator<(const optional_ref<T>& opt, const U& value) noexcept -> bool {
    return opt.has_value() && *opt < value;
}

template<typename U, typename T>
    requires(order_comparable_with<U, T> && is_not_specialisation_of<U, optional_ref>)
[[nodiscard]] auto operator<(const U& value, const optional_ref<T>& opt) noexcept -> bool {
    return opt.has_value() && value < *opt;
}

template<typename T, typename U>
    requires(order_comparable_with<T, U> && is_not_specialisation_of<U, optional_ref>)
[[nodiscard]] auto operator<=(const optional_ref<T>& opt, const U& value) noexcept -> bool {
    return opt.has_value() && *opt <= value;
}

template<typename U, typename T>
    requires(order_comparable_with<U, T> && is_not_specialisation_of<U, optional_ref>)
[[nodiscard]] auto operator<=(const U& value, const optional_ref<T>& opt) noexcept -> bool {
    return opt.has_value() && value <= *opt;
}

template<typename T, typename U>
    requires(order_comparable_with<T, U> && is_not_specialisation_of<U, optional_ref>)
[[nodiscard]] auto operator>(const optional_ref<T>& opt, const U& value) noexcept -> bool {
    return opt.has_value() && *opt > value;
}

template<typename U, typename T>
    requires(order_comparable_with<U, T> && is_not_specialisation_of<U, optional_ref>)
[[nodiscard]] auto operator>(const U& value, const optional_ref<T>& opt) noexcept -> bool {
    return opt.has_value() && value > *opt;
}

template<typename T, typename U>
    requires(order_comparable_with<T, U> && is_not_specialisation_of<U, optional_ref>)
[[nodiscard]] auto operator>=(const optional_ref<T>& opt, const U& value) noexcept -> bool {
    return opt.has_value() && *opt >= value;
}

template<typename U, typename T>
    requires(order_comparable_with<U, T> && is_not_specialisation_of<U, optional_ref>)
[[nodiscard]] auto operator>=(const U& value, const optional_ref<T>& opt) noexcept -> bool {
    return opt.has_value() && value >= *opt;
}

template<typename T, typename U>
    requires(std::three_way_comparable_with<T, U> && is_not_specialisation_of<U, optional_ref>)
[[nodiscard]] auto operator<=>(const optional_ref<T>& opt, const U& value) noexcept
    -> std::compare_three_way_result<T, U> {
    return opt.has_value() ? *opt <=> value : std::strong_ordering::less;
}
}// namespace serpp

#endif// #ifndef SERPP_OPTIONAL_REF_HPP
