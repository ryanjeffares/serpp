#include "serpp/parse/parse.hpp"

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
    "numbers": [0, 1, 2, 3]
})"sv;
    const auto result = parse::parse(json);
    if (!result) {
        std::cout << result.error().what() << std::endl;
    }
}
// } // namespace serpp::tests