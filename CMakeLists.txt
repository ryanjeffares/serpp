cmake_minimum_required(VERSION 3.30)
project(serpp)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY  https://github.com/catchorg/Catch.git
        GIT_TAG         v3.8.0
        GIT_SHALLOW     ON
)

FetchContent_MakeAvailable(Catch2)

add_library(
        serpp

        ${CMAKE_CURRENT_SOURCE_DIR}/src/json/detail/array.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/json/jarray.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/json/jobject.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/json/jvalue.cpp
)

if (MSVC)
    target_compile_options(serpp PRIVATE /W4 /WX)
else ()
    target_compile_options(serpp PRIVATE -Wall -Wextra -Wpedantic -Werror -Wconversion -Wswitch)
endif ()

target_include_directories(serpp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(tests serpp-tests)
