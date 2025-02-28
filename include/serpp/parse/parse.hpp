#ifndef SERPP_PARSE_HPP
#define SERPP_PARSE_HPP

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <expected>
#include <format>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

namespace serpp::parse {
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
        : m_message{ std::format("{}: {}", error_type_to_string(error_type), message) } {

    }

    [[nodiscard]] auto what() const noexcept -> const char* override {
        return m_message.c_str();
    }

private:
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
    while (std::isdigit(source[i]) != 0) {
        i++;
    }

    return std::make_optional<token>(token_type::number, poll_text(source, i));
}

[[nodiscard]] inline auto scan_string(std::string_view& source) noexcept -> std::optional<token> {
    for (std::size_t i = 1; i < source.size(); i++) {
        if (source[i] == '"' && source[i - 1] != '\\') {
            const auto text = source.substr(1, i - 1);
            source = source.substr(i + 1);

            if (text == "true") {
                return std::make_optional<token>(token_type::true_value, text);
            } else if (text == "false") {
                return std::make_optional<token>(token_type::false_value, text);
            } else if (text == "null") {
                return std::make_optional<token>(token_type::null, text);
            } else {
                return std::make_optional<token>(token_type::string, text);
            }
        }
    }

    return std::make_optional<token>(token_type::error, source.substr(0, 1));
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
            } else {
                return std::make_optional<token>(token_type::error, poll_text(source, 1));
            }
    }
}

[[nodiscard]] inline auto parse_array(std::string_view& source) noexcept -> std::expected<void, parse_error>;
[[nodiscard]] inline auto parse_object(std::string_view& source) noexcept -> std::expected<void, parse_error>;
[[nodiscard]] inline auto parse_value(std::string_view& source) noexcept -> std::expected<void, parse_error>;
[[nodiscard]] inline auto parse_value(std::string_view& source, token token) noexcept -> std::expected<void, parse_error>;

inline auto parse_array(std::string_view& source) noexcept -> std::expected<void, parse_error> {
    enum class last_element {
        comma,
        none,
        value,
    };

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

            return {};
        } else {
            auto value = parse_value(source, *token);
            if (!value) {
                return value;
            }

            last_element = last_element::value;
        }
    }

    // made it to EOF without finding a matching bracket
    return std::unexpected<parse_error>{
        std::in_place,
        error_type::syntax_error,
        "unterminated array, expected ']'",
    };
}

inline auto parse_object(std::string_view& source) noexcept -> std::expected<void, parse_error> {
    enum class last_element {
        comma,
        none,
        value,
    };

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

            return {};
        } else {
            if (token->token_type != token_type::string) {
                return std::unexpected<parse_error>{
                    std::in_place,
                    error_type::syntax_error,
                    std::format("expected string but got '{}'", token->text),
                };
            }

            const auto colon = scan_token(source);
            if (!colon) {
                return std::unexpected<parse_error>{
                    std::in_place,
                    error_type::syntax_error,
                    std::format("expected ':' but got '{}'", colon->text),
                };
            }
            if (colon->token_type != token_type::colon) {
                return std::unexpected<parse_error>{
                    std::in_place,
                    error_type::syntax_error,
                    std::format("expected ':' but got '{}'", colon->text),
                };
            }

            const auto value = parse_value(source);
            if (!value) {
                return value;
            }

            last_element = last_element::value;
        }
    }

    // made it to EOF without finding a matching bracket
    return std::unexpected<parse_error>{
        std::in_place,
        error_type::syntax_error,
        "unterminated object, expected '}'",
    };
}

inline auto parse_value(std::string_view& source) noexcept -> std::expected<void, parse_error> {
    if (const auto token = scan_token(source)) {
        return parse_value(source, *token);
    } else {
        return std::unexpected<parse_error>{
            std::in_place,
            error_type::syntax_error,
            "expected value but got end of text",
        };
    }
}

inline auto parse_value(std::string_view& source, token token) noexcept -> std::expected<void, parse_error> {
    switch (token.token_type) {
        case token_type::brace_left:
            return parse_object(source);
        case token_type::bracket_left:
            return parse_array(source);
        case token_type::false_value:
            std::cout << std::boolalpha << false << std::endl;
            return {};
        case token_type::number:
        case token_type::null:
        case token_type::string:
            std::cout << token.text << std::endl;
            return {};
        case token_type::true_value:
            std::cout << std::boolalpha << true << std::endl;
            return {};
        default:
            return std::unexpected<parse_error>{
                std::in_place,
                error_type::syntax_error,
                std::format("invalid value '{}'", token.text),
            };
    }
}
} // namespace detail

inline auto parse(std::string_view source) noexcept -> std::expected<void, parse_error> {
    using namespace detail;

    const auto first_token = scan_token(source);

    if (!first_token.has_value()) {
        return {};
    }

    switch (first_token->token_type) {
        case token_type::brace_left:
            return parse_object(source);
        case token_type::bracket_left:
            return parse_array(source);
        default:
            return std::unexpected<parse_error>{
                std::in_place,
                error_type::syntax_error,
                "expected '[' or '{' at start of JSON",
            };
    }
}
} // namespace serpp::parse

#endif // #ifndef SERPP_PARSE_HPP
