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
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
 * OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef SERPP_JSON_HPP
#define SERPP_JSON_HPP

#include <concepts>
#include <cstddef>
#include <optional>
#include <utility>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace serpp::json {
class json {
public:
    template<typename T>
    using optional_ref = std::optional<std::reference_wrapper<T>>;

    template<typename T>
    using optional_cref = std::optional<std::reference_wrapper<const T>>;

    enum class type {
        number,
        boolean,
        null,
        string,
        array,
        object,
    };

    template<typename T> requires std::convertible_to<T, double>
    /* explicit */ json(T&& value) : m_data{ static_cast<double>(std::forward<T>(value)) } {}

    /* explicit */ json(bool value) : m_data{ value } {}

    /* explicit */ json(std::nullptr_t value = nullptr) : m_data{ value } {}

    template<typename... Args> requires std::constructible_from<std::string, Args...>
    /* explicit */ json(Args&&... args) : m_data{ std::string{ std::forward<Args>(args)... } } {}

    template<typename... Args> requires std::constructible_from<std::vector<json>, Args...>
    /* explicit */ json(Args&&... args) : m_data{ std::vector<json>{ std::forward<Args>(args)... } } {}

    template<typename... Args> requires std::constructible_from<std::unordered_map<std::string, json>, Args...>
    /* explicit */ json(Args&&... args)
        : m_data{ std::unordered_map<std::string, json>{ std::forward<Args>(args)... } } {}

    [[nodiscard]] auto type() const noexcept -> type {
        return static_cast<enum type>(m_data.index());
    }

    [[nodiscard]] auto number() const noexcept -> std::optional<double> {
        if (type() == type::number) {
            return std::get<double>(m_data);
        } else {
            return std::nullopt;
        }
    }

    [[nodiscard]] auto number() noexcept ->std::optional<double> {
        if (type() == type::number) {
            return std::get<double>(m_data);
        } else {
            return std::nullopt;
        }
    }

    [[nodiscard]] auto boolean() const noexcept -> std::optional<bool> {
        if (type() == type::boolean) {
            return std::get<bool>(m_data);
        } else {
            return std::nullopt;
        }
    }

    [[nodiscard]] auto boolean() noexcept ->std::optional<bool> {
        if (type() == type::boolean) {
            return std::get<bool>(m_data);
        } else {
            return std::nullopt;
        }
    }

    [[nodiscard]] auto null() const noexcept -> std::optional<std::nullptr_t> {
        if (type() == type::null) {
            return std::get<std::nullptr_t>(m_data);
        } else {
            return std::nullopt;
        }
    }

    [[nodiscard]] auto null() noexcept ->std::optional<std::nullptr_t> {
        if (type() == type::null) {
            return std::get<std::nullptr_t>(m_data);
        } else {
            return std::nullopt;
        }
    }

    [[nodiscard]] auto string() const noexcept -> optional_cref<std::string> {
        if (type() == type::string) {
            return std::get<std::string>(m_data);
        } else {
            return std::nullopt;
        }
    }

    [[nodiscard]] auto string() noexcept -> optional_ref<std::string> {
        if (type() == type::string) {
            return std::get<std::string>(m_data);
        } else {
            return std::nullopt;
        }
    }

    [[nodiscard]] auto array() const noexcept -> optional_cref<std::vector<json>> {
        if (type() == type::array) {
            return std::get<std::vector<json>>(m_data);
        } else {
            return std::nullopt;
        }
    }

    [[nodiscard]] auto array() noexcept -> optional_ref<std::vector<json>> {
        if (type() == type::array) {
            return std::get<std::vector<json>>(m_data);
        } else {
            return std::nullopt;
        }
    }

    [[nodiscard]] auto object() const noexcept -> optional_cref<std::unordered_map<std::string, json>> {
        if (type() == type::object) {
            return std::get<std::unordered_map<std::string, json>>(m_data);
        } else {
            return std::nullopt;
        }
    }

    [[nodiscard]] auto object() noexcept -> optional_ref<std::unordered_map<std::string, json>> {
        if (type() == type::object) {
            return std::get<std::unordered_map<std::string, json>>(m_data);
        } else {
            return std::nullopt;
        }
    }

    template<typename T> requires (std::is_arithmetic_v<T> && !std::is_same_v<T, bool>)
    [[nodiscard]] auto get() const noexcept -> std::optional<T> {
        if (type() == type::number) {
            return static_cast<double>(std::get<double>(m_data));
        } else {
            return std::nullopt;
        }
    }

    template<typename T> requires std::same_as<T, bool>
    [[nodiscard]] auto get() const noexcept -> std::optional<bool> {
        if (type() == type::boolean) {
            return std::get<bool>(m_data);
        } else {
            return std::nullopt;
        }
    }

    template<typename T> requires std::constructible_from<T, std::string>
    [[nodiscard]] auto get() const noexcept -> std::optional<T> {
        if (type() == type::string) {
            return T{ std::get<std::string>(m_data) };
        } else {
            return std::nullopt;
        }
    }

private:
    std::variant<
        double,
        bool,
        std::nullptr_t,
        std::string,
        std::vector<json>,
        std::unordered_map<std::string, json>
    > m_data;
};
} // namespace serpp::json

#endif // #ifndef SERPP_JSON_HPP
