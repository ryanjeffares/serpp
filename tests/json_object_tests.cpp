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

#include "serpp/json/json.hpp"

#include <catch2/catch_test_macros.hpp>

namespace serpp::tests {
TEST_CASE("Basic JSON creation tests", "[json][creation]") {
    {
        json::json j{
            { "name", "Ryan Jeffares" },
            { "age", 25 },
            { "numbers", { 0, 1, 2, 3 } },
            { "gender", nullptr },
            { "alive", true },
            { "data", {
                { "foo", "bar"},
                { "baz", {
                    "Hello",
                    "World",
                    0,
                    1,
                }}
            }}
        };

        REQUIRE(j.type() == json::json::type::object);

        const auto& object = *j.object();
        REQUIRE(object.contains("name"));
        REQUIRE(*object.at("name") == "Ryan Jeffares");
        REQUIRE(object.contains("age"));
        REQUIRE(*object.at("age") == 25);
        REQUIRE(object.contains("numbers"));
        REQUIRE(*object.at("numbers") == std::vector<json::json>{ 0.0, 1.0, 2.0, 3.0 });
        REQUIRE(object.contains("gender"));
        REQUIRE(*object.at("gender") == nullptr);

        REQUIRE(object.contains("data"));
        const auto& data = *object.at("data");
        REQUIRE(data.type() == json::json::type::object);
        const auto& data_object = *data.object();
        REQUIRE(data_object.contains("foo"));
        REQUIRE(*data_object.at("foo") == "bar");
        REQUIRE(data_object.contains("baz"));
        REQUIRE(*data_object.at("baz") == std::vector<json::json>{ "Hello", "World", 0, 1 });
    }
}
} // namespace serpp::tests