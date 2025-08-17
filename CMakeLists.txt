cmake_minimum_required(VERSION 3.18)

project(lite)

file(GLOB HEADERS

)

file(GLOB SOURCES
    src_for_sdl3/main.c
)

add_subdirectory(.vendored_libs/SDL3 EXCLUDE_FROM_ALL)

add_executable(lite ${SOURCES})
target_link_libraries(lite PRIVATE SDL3::SDL3)