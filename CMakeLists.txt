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

add_subdirectory(tests serpp-tests)
