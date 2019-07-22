cmake_minimum_required(VERSION 3.8)
project(uvrestful)

option(UVRESTFUL_BUILD_TESTS "build tests" OFF)

include_directories("/usr/local/include")
link_directories("/usr/local/lib")

add_subdirectory(uvrestful)

if (UVRESTFUL_BUILD_TESTS)
    add_subdirectory(tests)
endif (UVRESTFUL_BUILD_TESTS)

