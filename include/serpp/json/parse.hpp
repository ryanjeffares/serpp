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

#ifndef SERPP_PARSE_HPP
#define SERPP_PARSE_HPP

#include "json.hpp"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <expected>
#include <format>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace serpp::json {
enum class error_type {
    expected_token,
    invalid_token,
    syntax_error,
};

inline auto error_type_to_string(error_type error_type) noexcept -> std::string_view {
    switch (error_type) {
        case error_type::expected_token:
            return "expected token";
        case error_type::invalid_token:
            return "invalid token";
        case error_type::syntax_error:
            return "syntax error";
    }

    return {};
}

class parse_error : public std::exception {
public:
    parse_error(error_type error_type, std::string_view message)
        : m_error_type{ error_type }
        , m_message{ std::format("{}: {}", error_type_to_string(error_type), message) } {}

    [[nodiscard]] auto what() const noexcept -> const char* override {
        return m_message.c_str();
    }

    [[nodiscard]] auto type() const noexcept -> error_type {
        return m_error_type;
    }

private:
    error_type m_error_type;
    std::string m_message;
};

namespace detail {
enum class token_type {
    // error
    error,

    // symbols
    brace_left,
    brace_right,
    bracket_left,
    bracket_right,
    colon,
    comma,

    // value literals
    false_value,
    number,
    null,
    string,
    true_value,
};

[[nodiscard]] constexpr inline auto token_type_to_string(token_type token_type) noexcept -> std::string_view {
    switch (token_type) {
        case token_type::error:
            return "error";
        case token_type::brace_left:
            return "brace_left";
        case token_type::brace_right:
            return "brace_right";
        case token_type::bracket_left:
            return "bracket_left";
        case token_type::bracket_right:
            return "bracket_right";
        case token_type::colon:
            return ":";
        case token_type::comma:
            return ",";
        case token_type::false_value:
            return "false_value";
        case token_type::number:
            return "number";
        case token_type::null:
            return "null";
        case token_type::string:
            return "string";
        case token_type::true_value:
            return "true_value";
    }

    return {};
}

struct token {
    token_type token_type;
    std::string_view text;
};

inline auto skip_whitespace(std::string_view& source) noexcept -> void {
    const auto it = std::ranges::find_if(source, [] (const auto c) {
        return std::isspace(c) == 0;
    });

    source = { it, source.end() };
}

inline auto poll_text(std::string_view& source, std::size_t len) noexcept -> std::string_view {
    const auto sv = source.substr(0, len);
    source = source.substr(len);
    return sv;
}

[[nodiscard]] inline auto scan_number(std::string_view& source) noexcept -> std::optional<token> {
    // TODO
    std::size_t i = 0;
    while (i < source.size() && std::isdigit(source[i])) {
        i++;
    }

    return std::make_optional<token>(token_type::number, poll_text(source, i));
}

[[nodiscard]] inline auto scan_string(std::string_view& source) noexcept -> std::optional<token> {
    for (std::size_t i = 1; i < source.size(); i++) {
        if (source[i] == '"' && source[i - 1] != '\\') {
            const auto text = source.substr(1, i - 1);
            source = source.substr(i + 1);
            return std::make_optional<token>(token_type::string, text);
        }
    }

    return std::make_optional<token>(token_type::error, source.substr(0, 1));
}

[[nodiscard]] inline auto scan_literal(std::string_view& source) noexcept -> std::optional<token> {
    std::size_t i = 0;
    while (i < source.size() && std::isalpha(source[i])) {
        i++;
    }

    const auto literal = poll_text(source, i);

    if (literal == "true") {
        return std::make_optional<token>(token_type::true_value, literal);
    } else if (literal == "false") {
        return std::make_optional<token>(token_type::false_value, literal);
    } else if (literal == "null") {
        return std::make_optional<token>(token_type::null, literal);
    } else {
        return std::make_optional<token>(token_type::error, literal);
    }
}

[[nodiscard]] inline auto scan_token(std::string_view& source) noexcept -> std::optional<token> {
    skip_whitespace(source);
    if (source.empty()) {
        return std::nullopt;
    }

    switch (source.front()) {
        case '[':
            return std::make_optional<token>(token_type::bracket_left, poll_text(source, 1));
        case ']':
            return std::make_optional<token>(token_type::bracket_right, poll_text(source, 1));
        case '{':
            return std::make_optional<token>(token_type::brace_left, poll_text(source, 1));
        case '}':
            return std::make_optional<token>(token_type::brace_right, poll_text(source, 1));
        case ':':
            return std::make_optional<token>(token_type::colon, poll_text(source, 1));
        case ',':
            return std::make_optional<token>(token_type::comma, poll_text(source, 1));
        case '"':
            return scan_string(source);
        default:
            if (std::isdigit(source.front())) {
                return scan_number(source);
            } else if (std::isalpha(source.front())) {
                return scan_literal(source);
            } else {
                return std::make_optional<token>(token_type::error, poll_text(source, 1));
            }
    }
}

[[nodiscard]] inline auto parse_array(std::string_view& source) noexcept -> std::expected<json, parse_error>;
[[nodiscard]] inline auto parse_object(std::string_view& source) noexcept -> std::expected<json, parse_error>;
[[nodiscard]] inline auto parse_value(std::string_view& source) noexcept -> std::expected<json, parse_error>;
[[nodiscard]] inline auto parse_value(
    std::string_view& source,
    const token& token
) noexcept -> std::expected<json, parse_error>;

inline auto parse_array(std::string_view& source) noexcept -> std::expected<json, parse_error> {
    enum class last_element {
        comma,
        none,
        value,
    };

    auto j = array();
    auto& array = j.array().get();
    last_element last_element = last_element::none;

    while (const auto token = scan_token(source)) {
        if (token->token_type == token_type::comma) {
            if (last_element != last_element::value) {
                return std::unexpected<parse_error>{
                    std::in_place,
                    error_type::syntax_error,
                    "no trailing commas allowed",
                };
            }

            last_element = last_element::comma;
        } else if (token->token_type == token_type::bracket_right) {
            if (last_element == last_element::comma) {
                return std::unexpected<parse_error>{
                    std::in_place,
                    error_type::syntax_error,
                    "no trailing commas allowed",
                };
            }

            return j;
        } else {
            auto value = parse_value(source, *token);
            if (!value) {
                return value;
            }
            array.emplace_back(std::move(*value));

            last_element = last_element::value;
        }
    }

    // made it to EOF without finding a matching bracket
    return std::unexpected<parse_error>{
        std::in_place,
        error_type::expected_token,
        "unterminated json, expected ']'",
    };
}

inline auto parse_object(std::string_view& source) noexcept -> std::expected<json, parse_error> {
    enum class last_element {
        comma,
        none,
        value,
    };

    auto j = object();
    auto& object = j.object().get();
    last_element last_element = last_element::none;

    while (const auto token = scan_token(source)) {
        if (token->token_type == token_type::comma) {
            if (last_element != last_element::value) {
                return std::unexpected<parse_error>{
                    std::in_place,
                    error_type::syntax_error,
                    "no trailing commas allowed",
                };
            }

            last_element = last_element::comma;
        } else if (token->token_type == token_type::brace_right) {
            if (last_element == last_element::comma) {
                return std::unexpected<parse_error>{
                    std::in_place,
                    error_type::syntax_error,
                    "no trailing commas allowed",
                };
            }

            return j;
        } else {
            if (token->token_type != token_type::string) {
                return std::unexpected<parse_error>{
                    std::in_place,
                    error_type::invalid_token,
                    std::format("expected string but got '{}'", token->text),
                };
            }

            const auto colon = scan_token(source);
            if (!colon) {
                return std::unexpected<parse_error>{
                    std::in_place,
                    error_type::invalid_token,
                    std::format("expected ':' but got '{}'", colon->text),
                };
            }
            if (colon->token_type != token_type::colon) {
                return std::unexpected<parse_error>{
                    std::in_place,
                    error_type::invalid_token,
                    std::format("expected ':' but got '{}'", colon->text),
                };
            }

            auto value = parse_value(source);
            if (!value) {
                return value;
            }

            object.insert_or_assign(token->text, std::move(*value));
            last_element = last_element::value;
        }
    }

    // made it to EOF without finding a matching bracket
    return std::unexpected<parse_error>{
        std::in_place,
        error_type::expected_token,
        "unterminated object, expected '}'",
    };
}

inline auto parse_value(std::string_view& source) noexcept -> std::expected<json, parse_error> {
    if (const auto token = scan_token(source)) {
        return parse_value(source, *token);
    } else {
        return std::unexpected<parse_error>{
            std::in_place,
            error_type::expected_token,
            "expected value but got end of text",
        };
    }
}

inline auto parse_value(std::string_view& source, const token& token) noexcept -> std::expected<json, parse_error> {
    switch (token.token_type) {
        case token_type::brace_left:
            return parse_object(source);
        case token_type::bracket_left:
            return parse_array(source);
        case token_type::false_value:
            return false;
        case token_type::number:
            return std::strtod(token.text.data(), nullptr);
        case token_type::null:
            return nullptr;
        case token_type::string:
            return std::string{ token.text };
        case token_type::true_value:
            return true;
        default:
            return std::unexpected<parse_error>{
                std::in_place,
                error_type::invalid_token,
                std::format("invalid value '{}'", token.text),
            };
    }
}
} // namespace detail

inline auto parse(std::string_view source) noexcept -> std::expected<json, parse_error> {
    using namespace detail;

    if (const auto first_token = scan_token(source)) {
        return parse_value(source, *first_token);
    } else {
        return {};
    }
}
} // namespace serpp::json

#endif // #ifndef SERPP_PARSE_HPP
