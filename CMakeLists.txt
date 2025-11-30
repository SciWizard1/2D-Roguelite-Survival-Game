cmake_minimum_required(VERSION 3.10)
project(Survival C)

set(CMAKE_C_STANDARD 99)

find_package(Python3 COMPONENTS Interpreter REQUIRED)

file(GLOB SHADER_FILES "${CMAKE_SOURCE_DIR}/src/*.glsl")

set(GENERATED_SOURCE "${CMAKE_SOURCE_DIR}/src/shaders_embedded.c")

add_custom_command(
    OUTPUT ${GENERATED_SOURCE}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/shader_source_generator.py
            ${CMAKE_SOURCE_DIR}/src        # input dir
            ${GENERATED_SOURCE}            # output header
    DEPENDS ${SHADER_FILES} ${CMAKE_SOURCE_DIR}/shader_source_generator.py
    COMMENT "Embedding GLSL shaders into ${GENERATED_SOURCE}"
)

add_custom_target(embed_shaders DEPENDS ${GENERATED_SOURCE})


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
    src/shaders_embedded.c
    src/gpu_data_transfer.c
)

add_dependencies(game embed_shaders)

target_link_libraries(game minifb GLEW glfw GL m)