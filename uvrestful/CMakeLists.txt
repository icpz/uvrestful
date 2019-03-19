cmake_minimum_required(VERSION 3.8)
project(uvrestful)

add_library(uthash INTERFACE)
target_include_directories(uthash INTERFACE uthash/include)

list(APPEND SRCS
    src/fields.c
    src/parser.c
    src/request.c
    src/response.c
    src/http_code.c
    src/http_code.h
    src/vstr_hash_map.c
    src/server.c
    src/server_conn.c
    )

list(APPEND DEPS uv uthash)

add_library(${PROJECT_NAME} STATIC ${SRCS})
target_include_directories(${PROJECT_NAME} PUBLIC include/)
target_link_libraries(${PROJECT_NAME} PUBLIC ${DEPS})

