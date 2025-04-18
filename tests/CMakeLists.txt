add_executable(
        tests

        ${CMAKE_CURRENT_SOURCE_DIR}/json/jarray_tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/json/jvalue_tests.cpp
)

if (MSVC)
    set(SERPP_TESTS_COMPILE_OPTIONS /W4 /WX)
else ()
    set(SERPP_TESTS_COMPILE_OPTIONS -Wall -Wextra -Wpedantic -Wconversion -Werror -Wswitch)
endif ()

target_compile_options(tests PRIVATE ${SERPP_TESTS_COMPILE_OPTIONS})
target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(tests PRIVATE serpp Catch2::Catch2WithMain)
