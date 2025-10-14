cmake_minimum_required(VERSION 3.10)
project(Survival C)

set(CMAKE_C_STANDARD 99)
add_subdirectory(minifb)

include_directories(minifb/include)

add_compile_options(-Wall -Wextra -Wpedantic -O3)

add_executable(game src/main.c src/chunks.c src/baserenderer.c)
target_link_libraries(game minifb)