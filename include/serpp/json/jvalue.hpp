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

#ifndef SERPP_JVALUE_HPP
#define SERPP_JVALUE_HPP

#include "jarray.hpp"
#include "jobject.hpp"
#include "serpp/macros.hpp"
#include "serpp/optional_ref.hpp"

#include <cstddef>
#include <string>
#include <type_traits>
#include <variant>

namespace serpp::json {
class jvalue {
public:
    enum class type {
        string,
        number,
        object,
        array,
        boolean,
        null,
    };

    template<typename T>
        requires std::is_constructible_v<std::string, T&&>
    jvalue(T&& string) noexcept(std::is_nothrow_constructible_v<std::string, T&&>)
        : m_data{std::string{std::forward<T>(string)}} {}

    template<typename T>
        requires(std::is_arithmetic_v<T> && !std::is_same_v<T, bool>)
    jvalue(const T& number) noexcept
        : m_data{static_cast<double>(number)} {}

    jvalue(const jobject& object);
    jvalue(jobject&& object) noexcept;

    jvalue(const jarray& array);
    jvalue(jarray&& array) noexcept;

    jvalue(bool boolean) noexcept;

    jvalue(std::nullptr_t null = nullptr) noexcept;

    SERPP_DECLARE_DEFAULT_COPYABLE(jvalue)

    jvalue(jvalue&& other) noexcept
        : m_data{std::move(other.m_data)} {
        other = nullptr;
    }

    auto operator=(jvalue&& other) noexcept -> jvalue& {
        if (this != &other) {
            m_data = std::move(other.m_data);
            other = nullptr;
        }

        return *this;
    }

    template<typename T>
        requires std::is_constructible_v<std::string, T&&>
    auto operator=(T&& string) noexcept(std::is_nothrow_constructible_v<std::string, T>) -> jvalue& {
        m_data.emplace<std::string>(std::forward<T>(string));
        return *this;
    }

    template<typename T>
        requires(std::is_arithmetic_v<T> && !std::is_same_v<T, bool>)
    auto operator=(const T& number) noexcept -> jvalue& {
        m_data.emplace<double>(static_cast<double>(number));
        return *this;
    }

    auto operator=(const jobject& object) -> jvalue&;
    auto operator=(jobject&& object) noexcept -> jvalue&;

    auto operator=(const jarray& array) -> jvalue&;
    auto operator=(jarray&& array) noexcept -> jvalue&;

    auto operator=(bool boolean) noexcept -> jvalue&;

    auto operator=(std::nullptr_t) noexcept -> jvalue&;

    auto operator==(const jvalue& other) const noexcept -> bool {
        if (type() != other.type()) {
            return false;
        }

        switch (type()) {
            case type::string:
                return string() == other.string();
            case type::number:
                return number() == other.number();
            case type::object:
                return object() == other.object();
            case type::array:
                return array() == other.array();
            case type::boolean:
                return boolean() == other.boolean();
            case type::null:
                return null() == other.null();
            default:
                return false;
        }
    }

    template<typename T>
        requires std::is_constructible_v<std::string, T&&>
    auto emplace(T&& string) noexcept(std::is_nothrow_constructible_v<std::string, T>) -> void {
        m_data.emplace<std::string>(std::forward<T>(string));
    }

    template<typename T>
        requires(std::is_arithmetic_v<T> && !std::is_same_v<T, bool>)
    auto emplace(const T& number) noexcept -> void {
        m_data.emplace<double>(static_cast<double>(number));
    }

    auto emplace(const jobject& object) -> void;
    auto emplace(jobject&& object) noexcept -> void;
    auto emplace(const jarray& array) -> void;
    auto emplace(jarray&& array) noexcept -> void;
    auto emplace(bool boolean) noexcept -> void;
    auto emplace(std::nullptr_t) noexcept -> void;

    [[nodiscard]] auto type() const noexcept -> type;

    [[nodiscard]] auto string() noexcept -> optional_ref<std::string>;
    [[nodiscard]] auto string() const noexcept -> optional_ref<const std::string>;
    [[nodiscard]] auto number() noexcept -> optional_ref<double>;
    [[nodiscard]] auto number() const noexcept -> optional_ref<const double>;
    [[nodiscard]] auto object() noexcept -> optional_ref<jobject>;
    [[nodiscard]] auto object() const noexcept -> optional_ref<const jobject>;
    [[nodiscard]] auto array() noexcept -> optional_ref<jarray>;
    [[nodiscard]] auto array() const noexcept -> optional_ref<const jarray>;
    [[nodiscard]] auto boolean() noexcept -> optional_ref<bool>;
    [[nodiscard]] auto boolean() const noexcept -> optional_ref<const bool>;
    [[nodiscard]] auto null() noexcept -> optional_ref<std::nullptr_t>;
    [[nodiscard]] auto null() const noexcept -> optional_ref<const std::nullptr_t>;

    [[nodiscard]] auto to_string() const noexcept -> std::string;

private:
    std::variant<std::string, double, jobject, jarray, bool, std::nullptr_t> m_data;
};

template<typename T>
constexpr auto operator==(const jvalue& l, const T& r) noexcept -> bool {
    if constexpr (std::equality_comparable_with<std::string, T>) {
        return l.type() == jvalue::type::string && l.string() == r;
    } else if constexpr (std::is_arithmetic_v<T> && !std::is_same_v<T, bool>) {
        return l.type() == jvalue::type::number && l.number() == static_cast<double>(r);
    } else if constexpr (std::is_same_v<T, jobject>) {
        return l.type() == jvalue::type::object && l.object() == r;
    } else if constexpr (std::is_same_v<T, jarray>) {
        return l.type() == jvalue::type::array && l.array() == r;
    } else if constexpr (std::is_same_v<T, bool>) {
        return l.type() == jvalue::type::boolean && l.boolean() == r;
    } else if constexpr (std::equality_comparable_with<std::nullptr_t, T>) {
        return l.type() == jvalue::type::null && l.null() == r;
    } else {
        static_assert(!sizeof(T), "Cannot compare provided T to jvalue");
    }
}

template<typename T>
constexpr auto operator==(const T& l, const jvalue& r) noexcept -> bool {
    if constexpr (std::equality_comparable_with<std::string, T>) {
        return r.type() == jvalue::type::string && l == r.string();
    } else if constexpr (std::is_arithmetic_v<T> && !std::is_same_v<T, bool>) {
        return r.type() == jvalue::type::number && static_cast<double>(l) == r.number();
    } else if constexpr (std::is_same_v<T, jobject>) {
        return r.type() == jvalue::type::object && l == r.object();
    } else if constexpr (std::is_same_v<T, jarray>) {
        return r.type() == jvalue::type::array && l == r.array();
    } else if constexpr (std::is_same_v<T, bool>) {
        return r.type() == jvalue::type::boolean && l == r.boolean();
    } else if constexpr (std::equality_comparable_with<std::nullptr_t, T>) {
        return r.type() == jvalue::type::null && l == r.null();
    } else {
        static_assert(!sizeof(T), "Cannot compare provided T to jvalue");
    }
}

template<typename T>
constexpr auto operator!=(const jvalue& l, const T& r) noexcept -> bool {
    return !(l == r);
}
template<typename T>
constexpr auto operator!=(const T& l, const jvalue& r) noexcept -> bool {
    return !(l == r);
}

template<typename T>
constexpr auto operator<(const jvalue& l, const T& r) noexcept -> bool {
    if constexpr (order_comparable_with<std::string, T>) {
        return l.type() == jvalue::type::string && l.string() < r;
    } else if constexpr (std::is_arithmetic_v<T> && !std::is_same_v<T, bool>) {
        return l.type() == jvalue::type::number && l.number() < static_cast<double>(r);
    } else if constexpr (std::is_same_v<T, jarray>) {
        return l.type() == jvalue::type::array && l.array() < r;
    } else if constexpr (std::is_same_v<T, bool>) {
        return l.type() == jvalue::type::boolean && l.boolean() < r;
    } else if constexpr (order_comparable_with<std::nullptr_t, T>) {
        return l.type() == jvalue::type::null && l.null() < r;
    } else {
        static_assert(!sizeof(T), "Cannot compare provided T to jvalue");
    }
}

template<typename T>
constexpr auto operator<(const T& l, const jvalue& r) noexcept -> bool {
    if constexpr (order_comparable_with<std::string, T>) {
        return r.type() == jvalue::type::string && l < r.string();
    } else if constexpr (std::is_arithmetic_v<T> && !std::is_same_v<T, bool>) {
        return r.type() == jvalue::type::number && static_cast<double>(l) < r.number();
    } else if constexpr (std::is_same_v<T, jarray>) {
        return r.type() == jvalue::type::array && l < r.array();
    } else if constexpr (std::is_same_v<T, bool>) {
        return r.type() == jvalue::type::boolean && l < r.boolean();
    } else if constexpr (order_comparable_with<std::nullptr_t, T>) {
        return r.type() == jvalue::type::null && l < r.null();
    } else {
        static_assert(!sizeof(T), "Cannot compare provided T to jvalue");
    }
}

template<typename T>
constexpr auto operator<=(const jvalue& l, const T& r) noexcept -> bool {
    if constexpr (order_comparable_with<std::string, T>) {
        return l.type() == jvalue::type::string && l.string() <= r;
    } else if constexpr (std::is_arithmetic_v<T> && !std::is_same_v<T, bool>) {
        return l.type() == jvalue::type::number && l.number() <= static_cast<double>(r);
    } else if constexpr (std::is_same_v<T, jarray>) {
        return l.type() == jvalue::type::array && l.array() <= r;
    } else if constexpr (std::is_same_v<T, bool>) {
        return l.type() == jvalue::type::boolean && l.boolean() <= r;
    } else if constexpr (order_comparable_with<std::nullptr_t, T>) {
        return l.type() == jvalue::type::null && l.null() <= r;
    } else {
        static_assert(!sizeof(T), "Cannot compare provided T to jvalue");
    }
}

template<typename T>
constexpr auto operator<=(const T& l, const jvalue& r) noexcept -> bool {
    if constexpr (order_comparable_with<std::string, T>) {
        return r.type() == jvalue::type::string && l <= r.string();
    } else if constexpr (std::is_arithmetic_v<T> && !std::is_same_v<T, bool>) {
        return r.type() == jvalue::type::number && static_cast<double>(l) <= r.number();
    } else if constexpr (std::is_same_v<T, jarray>) {
        return r.type() == jvalue::type::array && l <= r.array();
    } else if constexpr (std::is_same_v<T, bool>) {
        return r.type() == jvalue::type::boolean && l <= r.boolean();
    } else if constexpr (order_comparable_with<std::nullptr_t, T>) {
        return r.type() == jvalue::type::null && l <= r.null();
    } else {
        static_assert(!sizeof(T), "Cannot compare provided T to jvalue");
    }
}

template<typename T>
constexpr auto operator>(const jvalue& l, const T& r) noexcept -> bool {
    if constexpr (order_comparable_with<std::string, T>) {
        return l.type() == jvalue::type::string && l.string() > r;
    } else if constexpr (std::is_arithmetic_v<T> && !std::is_same_v<T, bool>) {
        return l.type() == jvalue::type::number && l.number() > static_cast<double>(r);
    } else if constexpr (std::is_same_v<T, jarray>) {
        return l.type() == jvalue::type::array && l.array() > r;
    } else if constexpr (std::is_same_v<T, bool>) {
        return l.type() == jvalue::type::boolean && l.boolean() > r;
    } else if constexpr (order_comparable_with<std::nullptr_t, T>) {
        return l.type() == jvalue::type::null && l.null() > r;
    } else {
        static_assert(!sizeof(T), "Cannot compare provided T to jvalue");
    }
}

template<typename T>
constexpr auto operator>(const T& l, const jvalue& r) noexcept -> bool {
    if constexpr (order_comparable_with<std::string, T>) {
        return r.type() == jvalue::type::string && l > r.string();
    } else if constexpr (std::is_arithmetic_v<T> && !std::is_same_v<T, bool>) {
        return r.type() == jvalue::type::number && static_cast<double>(l) > r.number();
    } else if constexpr (std::is_same_v<T, jarray>) {
        return r.type() == jvalue::type::array && l > r.array();
    } else if constexpr (std::is_same_v<T, bool>) {
        return r.type() == jvalue::type::boolean && l > r.boolean();
    } else if constexpr (order_comparable_with<std::nullptr_t, T>) {
        return r.type() == jvalue::type::null && l > r.null();
    } else {
        static_assert(!sizeof(T), "Cannot compare provided T to jvalue");
    }
}

template<typename T>
constexpr auto operator>=(const jvalue& l, const T& r) noexcept -> bool {
    if constexpr (order_comparable_with<std::string, T>) {
        return l.type() == jvalue::type::string && l.string() >= r;
    } else if constexpr (std::is_arithmetic_v<T> && !std::is_same_v<T, bool>) {
        return l.type() == jvalue::type::number && l.number() >= static_cast<double>(r);
    } else if constexpr (std::is_same_v<T, jarray>) {
        return l.type() == jvalue::type::array && l.array() >= r;
    } else if constexpr (std::is_same_v<T, bool>) {
        return l.type() == jvalue::type::boolean && l.boolean() >= r;
    } else if constexpr (order_comparable_with<std::nullptr_t, T>) {
        return l.type() == jvalue::type::null && l.null() >= r;
    } else {
        static_assert(!sizeof(T), "Cannot compare provided T to jvalue");
    }
}

template<typename T>
constexpr auto operator>=(const T& l, const jvalue& r) noexcept -> bool {
    if constexpr (order_comparable_with<std::string, T>) {
        return r.type() == jvalue::type::string && l >= r.string();
    } else if constexpr (std::is_arithmetic_v<T> && !std::is_same_v<T, bool>) {
        return r.type() == jvalue::type::number && static_cast<double>(l) >= r.number();
    } else if constexpr (std::is_same_v<T, jarray>) {
        return r.type() == jvalue::type::array && l >= r.array();
    } else if constexpr (std::is_same_v<T, bool>) {
        return r.type() == jvalue::type::boolean && l >= r.boolean();
    } else if constexpr (order_comparable_with<std::nullptr_t, T>) {
        return r.type() == jvalue::type::null && l >= r.null();
    } else {
        static_assert(!sizeof(T), "Cannot compare provided T to jvalue");
    }
}
}// namespace serpp::json

#endif// #ifndef SERPP_JVALUE_HPP
