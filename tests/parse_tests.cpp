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

// #include <catch2/catch_test_macros.hpp>

#include <string_view>

// namespace serpp::tests {
int main() {
    using namespace serpp;
    using namespace std::string_view_literals;

    const auto json = R"(
{
    "name": "Ryan Jeffares",
    "age": 25,
    "numbers": [0, 1, 2, 3],
    "gender": "null",
    "alive": "true"
})"sv;
    auto result = json::parse(json);
    if (!result) {
        std::cout << result.error().what() << std::endl;
    }

    result->object()->get()["name"] = "hello";
    std::cout << *result->object()->get()["name"].get<std::string_view>() << std::endl;
}
// } // namespace serpp::tests