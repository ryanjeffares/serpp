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

#include "serpp/json/jarray.hpp"
#include "serpp/json/jvalue.hpp"

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <numeric>
#include <ranges>

namespace serpp::tests {
using namespace json;

TEST_CASE("Test jarray", "[json]") {
    SECTION("Basic construction and access") {
        jarray empty;
        REQUIRE(empty.empty());
        REQUIRE(empty.size() == 0);
        REQUIRE(empty.data() == nullptr);
        REQUIRE(!empty.front().has_value());

        jarray one(1);
        REQUIRE(one.size() == 1);
        REQUIRE(one.data() != nullptr);
        REQUIRE(one.front().has_value());
        REQUIRE(one.front()->type() == jvalue::type::null);

        jarray many(20);
        REQUIRE(many.size() == 20);
        for (std::size_t i = 0; i < 20; i++) {
            const auto value = many.at(i);
            REQUIRE(value.has_value());
            REQUIRE(value->type() == jvalue::type::null);
        }

        jarray many_values(20, "Hello");
        REQUIRE(many_values.size() == 20);
        for (std::size_t i = 0; i < 20; i++) {
            const auto& value = many_values[i];
            REQUIRE(value.type() == jvalue::type::string);
            REQUIRE(value == "Hello");
        }

        std::vector<jvalue> vector{0, 1, 2, 3, false, "Hello", jarray{}};
        jarray from_vector = vector;
        REQUIRE(from_vector.size() == vector.size());
        for (std::size_t i = 0; i < from_vector.size(); i++) {
            REQUIRE(from_vector[i] == vector[i]);
        }
    }

    SECTION("Basic modification") {
        jarray array;
        std::vector<jvalue> vector{0, 1, 2, 3, false, "Hello", jarray{}};
        for (const auto& i: vector) {
            array.push_back(i);
        }

        REQUIRE(array.size() == vector.size());
        for (std::size_t i = 0; i < array.size(); i++) {
            REQUIRE(array[i] == vector[i]);
        }

        jvalue a{"Hello"}, b{jobject{}};
        array.push_back(std::move(a));
        array.push_back(std::move(b));
        REQUIRE(a.type() == jvalue::type::null);
        REQUIRE(b.type() == json::jvalue::type::null);
        REQUIRE(array[array.size() - 2] == "Hello");
        REQUIRE(array[array.size() - 1] == jobject{});

        array.pop_back();
        REQUIRE(array.size() == 8);
        const auto last = array.pop();
        REQUIRE(last == "Hello");

        array.resize(10);
        REQUIRE(array.size() == 10);
        for (std::size_t i = 7; i < 10; i++) {
            REQUIRE(array[i] == nullptr);
        }

        array.resize(20, "Hello");
        REQUIRE(array.size() == 20);
        for (std::size_t i = 10; i < 20; i++) {
            REQUIRE(array[i] == "Hello");
        }

        array.resize(5);
        REQUIRE(array.size() == 5);
        array.shrink_to_fit();
        REQUIRE(array.capacity() == array.size());

        array.clear();
        REQUIRE(array.empty());
        REQUIRE(array.data() != nullptr);
        REQUIRE(array.capacity() == 5);

        array.assign(10, "Goodbye");
        REQUIRE(array.size() == 10);
        for (std::size_t i = 0; i < 10; i++) {
            REQUIRE(array[i] == "Goodbye");
        }

        array.assign({0, 1, 2, 3, 4, 5});
        REQUIRE(array.size() == 6);
        for (std::size_t i = 0; i < 6; i++) {
            REQUIRE(array[i] == i);
        }
    }

    SECTION("Iteration") {
        jarray array;
        REQUIRE(array.begin() == array.end());
        for (std::size_t i = 0; i < 20; i++) {
            array.emplace_back(i);
        }
        for (std::size_t i = 0; const auto& value: array) {
            REQUIRE(i++ == value);
        }
        int i = 0;
        for (auto it = array.begin(); it != array.end(); it++) {
            REQUIRE(*it == i++);
        }
        REQUIRE(std::accumulate(array.begin(), array.end(), 0, [](const auto acc, const auto& value) {
                    return acc + *value.number();
                }) == 190);
        REQUIRE(std::ranges::all_of(array, [](const auto& value) { return *value.number() >= 0; }));

        // clang-format off
        const auto s1 = *std::ranges::fold_left_first(
            std::views::transform(array, [](const jvalue& value) { return static_cast<int>(*value.number()); }) |
            std::views::transform([](int value) { return std::to_string(value); }),
            std::plus<>{}
        );
        REQUIRE(s1 == "012345678910111213141516171819");

        std::string s2;
        for (auto it = array.crbegin(); it != array.crend(); it++) {
            s2 += std::to_string(static_cast<int>(it->number()));
        }
        REQUIRE(s2 == "191817161514131211109876543210");

        s2.clear();

        for (const auto& s: std::ranges::reverse_view(array) |
                            std::views::transform([](const jvalue& value) {
                                return static_cast<int>(*value.number());
                            }) |
                            std::views::transform([](int value) { return std::to_string(value); })) {
            s2 += s;
        }
        REQUIRE(s2 == "191817161514131211109876543210");
        // clang-format on
    }
}
}// namespace serpp::tests
