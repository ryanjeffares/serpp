add_executable(tests parse_tests.cpp)

target_include_directories(tests PRIVATE ../include)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_compile_options(tests PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Werror -Wswitch)
