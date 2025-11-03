cmake_minimum_required(VERSION 3.10)
project(Survival C)

set(CMAKE_C_STANDARD 99)

add_subdirectory(minifb)
add_subdirectory(glew-cmake)
add_subdirectory(glfw)

include_directories(minifb/include)
include_directories(glew-cmake/include)
include_directories(glfw/include)

add_compile_options(-Wall -Wextra -Wpedantic -O3)

add_executable(game 
    src/main.c 
    src/chunks.c 
    src/baserenderer.c 
    src/save_file_manager.c 
    src/string_utilities.c 
    src/noise_functions.c 
    src/entities.c
    src/controls.c
)

target_link_libraries(game minifb GLEW glfw GL m)