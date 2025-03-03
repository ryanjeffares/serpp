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
#include <initializer_list>
#include <optional>
#include <utility>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace serpp::json {
template<typename T>
class optional_ref {
public:
    optional_ref() = default;
    /* implicit */ optional_ref(T& value) : m_value{ &value } {}

    [[nodiscard]] auto operator=(const optional_ref& other) noexcept -> optional_ref& {
        if (this != &other) {
            m_value = other.m_value;
        }

        return *this;
    }

    [[nodiscard]] auto has_value() const noexcept -> bool {
        return m_value != nullptr;
    }

    [[nodiscard]] operator bool() const noexcept {
        return has_value();
    }

    [[nodiscard]] auto get() const noexcept -> T& {
        return *m_value;
    }

    [[nodiscard]] operator T&() const noexcept {
        return get();
    }

    [[nodiscard]] auto operator*() const noexcept -> T& {
        return get();
    }

    [[nodiscard]] auto operator->() const noexcept -> T* {
        return m_value;
    }

private:
    T* m_value;
};

class json {
public:
    enum class type {
        null,
        number,
        boolean,
        string,
        array,
        object,
    };

    json() = default;

    template<typename T> requires std::convertible_to<T, double>
    /* implicit */ json(T&& value) : m_data{ static_cast<double>(std::forward<T>(value)) } {}

    /* implicit */ json(bool value) : m_data{ value } {}

    /* implicit */ json(std::nullptr_t value) : m_data{ value } {}

    template<typename... Args> requires std::constructible_from<std::string, Args...>
    /* implicit */ json(Args&&... args) : m_data{ std::string{ std::forward<Args>(args)... } } {}

    /* implicit */ json(std::initializer_list<json> elems) : m_data{ std::vector<json>{ elems } } {}

    /* implicit */ json(std::initializer_list<std::pair<const std::string, json>> pairs)
        : m_data{ std::unordered_map<std::string, json>{ pairs } } {}

    // TODO: not actually noexcept as long as we are using std::unordered_map
    json(const json&) noexcept = default;
    json(json&&) noexcept = default;
    auto operator=(const json&) noexcept -> json& = default;
    auto operator=(json&&) noexcept -> json& = default;

    [[nodiscard]] auto operator==(const json& other) const noexcept -> bool {
        return m_data == other.m_data;
    }

    [[nodiscard]] auto type() const noexcept -> type {
        return static_cast<enum type>(m_data.index());
    }

    [[nodiscard]] auto number() const noexcept -> optional_ref<const double> {
        if (type() == type::number) {
            return std::get<double>(m_data);
        } else {
            return {};
        }
    }

    [[nodiscard]] auto number() noexcept ->optional_ref<double> {
        if (type() == type::number) {
            return std::get<double>(m_data);
        } else {
            return {};
        }
    }

    [[nodiscard]] auto boolean() const noexcept -> optional_ref<const bool> {
        if (type() == type::boolean) {
            return std::get<bool>(m_data);
        } else {
            return {};
        }
    }

    [[nodiscard]] auto boolean() noexcept ->optional_ref<bool> {
        if (type() == type::boolean) {
            return std::get<bool>(m_data);
        } else {
            return {};
        }
    }

    [[nodiscard]] auto null() const noexcept -> optional_ref<const std::nullptr_t> {
        if (type() == type::null) {
            return std::get<std::nullptr_t>(m_data);
        } else {
            return {};
        }
    }

    [[nodiscard]] auto null() noexcept -> optional_ref<std::nullptr_t> {
        if (type() == type::null) {
            return std::get<std::nullptr_t>(m_data);
        } else {
            return {};
        }
    }

    [[nodiscard]] auto string() const noexcept -> optional_ref<const std::string> {
        if (type() == type::string) {
            return std::get<std::string>(m_data);
        } else {
            return {};
        }
    }

    [[nodiscard]] auto string() noexcept -> optional_ref<std::string> {
        if (type() == type::string) {
            return std::get<std::string>(m_data);
        } else {
            return {};
        }
    }

    [[nodiscard]] auto array() const noexcept -> optional_ref<const std::vector<json>> {
        if (type() == type::array) {
            return std::get<std::vector<json>>(m_data);
        } else {
            return {};
        }
    }

    [[nodiscard]] auto array() noexcept -> optional_ref<std::vector<json>> {
        if (type() == type::array) {
            return std::get<std::vector<json>>(m_data);
        } else {
            return {};
        }
    }

    [[nodiscard]] auto object() const noexcept -> optional_ref<const std::unordered_map<std::string, json>> {
        if (type() == type::object) {
            return std::get<std::unordered_map<std::string, json>>(m_data);
        } else {
            return {};
        }
    }

    [[nodiscard]] auto object() noexcept -> optional_ref<std::unordered_map<std::string, json>> {
        if (type() == type::object) {
            return std::get<std::unordered_map<std::string, json>>(m_data);
        } else {
            return {};
        }
    }

    template<typename T> requires (std::is_arithmetic_v<T> && !std::is_same_v<T, bool>)
    [[nodiscard]] auto get() const noexcept -> std::optional<T> {
        if (type() == type::number) {
            return static_cast<T>(std::get<double>(m_data));
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
        std::nullptr_t,
        double,
        bool,
        std::string,
        std::vector<json>,
        std::unordered_map<std::string, json>
    > m_data;
};

inline auto array() noexcept -> json {
    return json{ std::initializer_list<json>{} };
}

inline auto object() noexcept -> json {
    return json{ std::initializer_list<std::pair<const std::string, json>>{} };
}
} // namespace serpp::json

#endif // #ifndef SERPP_JSON_HPP
