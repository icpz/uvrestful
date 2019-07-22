cmake_minimum_required(VERSION 3.8)
project(uvrestful-tests)

add_executable(uvrtest uvrtest.c)
target_link_libraries(uvrtest uvrestful)

