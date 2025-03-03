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

#include "serpp/json/parse.hpp"

#include <catch2/catch_test_macros.hpp>

#include <string_view>

namespace serpp::tests {
TEST_CASE("Basic well-formed JSON parsing test", "[json][parsing]") {
    using namespace std::string_view_literals;

    {
        json::json j{ 0, 1, 2 };
        REQUIRE(j.type() == json::json::type::array);
    }

    {
        json::json j = { { "foo", "bar" } };
    }

    {
        const auto json_string = R"(
{
}
)"sv;

        const auto result = json::parse(json_string);
        REQUIRE(result.has_value());
        const auto& json = *result;
        REQUIRE(json.type() == json::json::type::object);
        REQUIRE(json.object().get().empty());
    }

    {
        const auto json_string = R"(
[
]
)"sv;

        const auto result = json::parse(json_string);
        REQUIRE(result.has_value());
        const auto& json = *result;
        REQUIRE(json.type() == json::json::type::array);
        REQUIRE(json.array().get().empty());
    }

    {
        const auto json_string = ""sv;
        const auto result = json::parse(json_string);
        REQUIRE(result.has_value());
        const auto& json = *result;
        REQUIRE(json.type() == json::json::type::null);
        REQUIRE(json.null().get() == nullptr);
    }

    {
        const auto json_string = R"(
{
    "name": "Ryan Jeffares",
    "age": 25,
    "numbers": [0, 1, 2, 3],
    "gender": null,
    "alive": true,
    "data": {
        "foo": "bar",
        "baz": [
            "Hello",
            "World",
            0,
            1
        ]
    }
})"sv;

        const auto result = json::parse(json_string);
        REQUIRE(result.has_value());
        const auto& json = *result;
        REQUIRE(json.type() == json::json::type::object);
        const auto object = json.object();
        REQUIRE(object.has_value());

        const auto& map = *object;

        REQUIRE(map.contains("name"));
        REQUIRE(map.at("name").type() == json::json::type::string);
        REQUIRE(map.at("name").string().get() == "Ryan Jeffares");
        REQUIRE(map.at("name").get<std::string>() == "Ryan Jeffares");
        REQUIRE(map.at("name").get<std::string_view>() == "Ryan Jeffares");

        REQUIRE(map.contains("age"));
        REQUIRE(map.at("age").type() == json::json::type::number);
        REQUIRE(map.at("age").number().get() == 25.0);
        REQUIRE(map.at("age").get<double>() == 25.0);
        REQUIRE(map.at("age").get<int>() == 25);

        REQUIRE(map.contains("numbers"));
        REQUIRE(map.at("numbers").type() == json::json::type::array);
        REQUIRE(map.at("numbers").array().get() == std::vector<json::json>{0.0, 1.0, 2.0, 3.0});

        REQUIRE(map.contains("gender"));
        REQUIRE(map.at("gender").type() == json::json::type::null);
        REQUIRE(map.at("gender").null().get() == nullptr);

        REQUIRE(map.contains("alive"));
        REQUIRE(map.at("alive").type() == json::json::type::boolean);
        REQUIRE(map.at("alive").boolean().get() == true);
        REQUIRE(map.at("alive").get<bool>() == true);

        REQUIRE(map.contains("data"));
        REQUIRE(map.at("data").type() == json::json::type::object);
        const auto& data = map.at("data").object().get();
        REQUIRE(data.at("foo").type() == json::json::type::string);
        REQUIRE(data.at("foo").string().get() == "bar");
        REQUIRE(data.at("baz").type() == json::json::type::array);
        REQUIRE(data.at("baz").array().get() == std::vector<json::json>{"Hello", "World", 0.0, 1.0});
    }
}

TEST_CASE("Basic ill-formed JSON parsing test", "[json][parsing]") {
    using namespace std::string_view_literals;

    {
        const auto json_string = "a"sv;
        const auto result = json::parse(json_string);
        REQUIRE(!result.has_value());
        REQUIRE(result.error().type() == json::error_type::invalid_token);
    }

    {
        const auto json_string = "[0,]"sv;
        const auto result = json::parse(json_string);
        REQUIRE(!result.has_value());
        REQUIRE(result.error().type() == json::error_type::syntax_error);
    }

    {
        const auto json_string = "{"sv;
        const auto result = json::parse(json_string);
        REQUIRE(!result.has_value());
        REQUIRE(result.error().type() == json::error_type::expected_token);
    }
}
} // namespace serpp::tests