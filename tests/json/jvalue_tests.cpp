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

#include "serpp/json/jvalue.hpp"

#include <catch2/catch_test_macros.hpp>

namespace serpp::tests {
using namespace json;

TEST_CASE("Test jvalue", "[json]") {
    SECTION("Basic construction") {
        jvalue string{"Hello"};
        REQUIRE(string.type() == jvalue::type::string);
        REQUIRE(string.string().has_value());
        REQUIRE(string == "Hello");
        string = std::string{"World"};
        REQUIRE(string.type() == jvalue::type::string);
        REQUIRE(string.string().has_value());
        REQUIRE(string == "World");
        string = std::string_view{"Goodbye"};
        REQUIRE(string.type() == jvalue::type::string);
        REQUIRE(string.string().has_value());
        REQUIRE(string == "Goodbye");
        jvalue string2 = string;
        REQUIRE(string2.type() == jvalue::type::string);
        REQUIRE(string2.string().has_value());
        REQUIRE(string2 == "Goodbye");
        jvalue string3 = std::move(string2);
        REQUIRE(string3.type() == jvalue::type::string);
        REQUIRE(string3.string().has_value());
        REQUIRE(string3 == "Goodbye");
        REQUIRE(string2.type() == jvalue::type::null);
        REQUIRE(string2.null().has_value());

        jvalue number{10};
        REQUIRE(number.type() == jvalue::type::number);
        REQUIRE(number.number().has_value());
        REQUIRE(number == 10);
        number = 3.14f;
        REQUIRE(number.type() == jvalue::type::number);
        REQUIRE(number.number().has_value());
        REQUIRE(number == 3.14f);
        number = 1e-23;
        REQUIRE(number.type() == jvalue::type::number);
        REQUIRE(number.number().has_value());
        REQUIRE(number == 1e-23);

        jvalue object{jobject{}};
        REQUIRE(object.type() == jvalue::type::object);
        REQUIRE(object.object().has_value());

        jvalue array{jarray{}};
        REQUIRE(array.type() == jvalue::type::array);
        REQUIRE(array.array().has_value());

        jvalue boolean{ true };
        REQUIRE(boolean.type() == jvalue::type::boolean);
        REQUIRE(boolean.boolean().has_value());
        REQUIRE(boolean == true);
        boolean = false;
        REQUIRE(boolean.type() == jvalue::type::boolean);
        REQUIRE(boolean.boolean().has_value());
        REQUIRE(boolean == false);
    }
}
}// namespace serpp::tests
