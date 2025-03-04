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

#include "serpp/json/hash_map.hpp"
#include "serpp/json/json.hpp"

#include <catch2/catch_test_macros.hpp>

namespace serpp::tests {
TEST_CASE("JSON hash map tests", "[json][data-structures]") {
    json::detail::hash_map<json::json> map;

    {
        auto [it, inserted] = map.insert("name", "Ryan Jeffares");
        REQUIRE(it != map.end());
        REQUIRE(inserted);
        REQUIRE(it->second.type() == json::json::type::string);
        REQUIRE(*it->second.string() == "Ryan Jeffares");
        REQUIRE(map["name"] == "Ryan Jeffares");
    }

    {
        auto [it, inserted] = map.insert("age", 25);
        REQUIRE(it != map.end());
        REQUIRE(inserted);
        REQUIRE(it->second.type() == json::json::type::number);
        REQUIRE(*it->second.number() == 25.0);
        REQUIRE(map["age"] == 25);
    }

    REQUIRE(map.capacity() == 8);
    REQUIRE(map.size() == 2);
    REQUIRE(map.contains("name"));
    REQUIRE(map.contains("age"));
    REQUIRE(map.capacity() == 8);
    REQUIRE(map.size() == 2);

    for (auto i = 0; i < 50; i++) {
        auto [it, inserted] = map.insert(std::to_string(i), i);
        REQUIRE(it != map.end());
        REQUIRE(inserted);
        REQUIRE(it->second.type() == json::json::type::number);
        REQUIRE(*it->second.number() == i);
        REQUIRE(map[std::to_string(i)] == i);
    }

    for (auto i = 0; i < 50; i++) {
        auto [it, inserted] = map.insert(std::to_string(i), i);
        REQUIRE(!inserted);
    }

    for (auto i = 0; i < 50; i++) {
        auto [it, inserted] = map.insert_or_assign(std::to_string(i), i * 2);
        REQUIRE(it != map.end());
        REQUIRE(!inserted);
        REQUIRE(it->second.type() == json::json::type::number);
        REQUIRE(*it->second.number() == i * 2);
        REQUIRE(map[std::to_string(i)] == i * 2);
    }

    REQUIRE(map.size() == 52);
    REQUIRE(map.capacity() == 128);
}
} // namespace serpp::tests