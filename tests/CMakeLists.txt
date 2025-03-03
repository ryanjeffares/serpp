add_executable(tests json_parse_tests.cpp)

target_include_directories(tests PRIVATE ../include)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

if (MSVC)
    set(SERPP_TESTS_COMPILE_OPTIONS /W4 /WX)
else ()
    set(SERPP_TESTS_COMPILE_OPTIONS -Wall -Wextra -Wpedantic -Wconversion -Werror -Wswitch)
endif ()

target_compile_options(tests PRIVATE ${SERPP_TESTS_COMPILE_OPTIONS})
