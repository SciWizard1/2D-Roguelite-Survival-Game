cmake_minimum_required(VERSION 3.10)
project(Survival C)

set(CMAKE_C_STANDARD 99)

find_package(Python3 COMPONENTS Interpreter REQUIRED)

set(SOURCE "src_new")

file(GLOB SHADER_FILES "${CMAKE_SOURCE_DIR}/${SOURCE}/*.glsl")

set(GENERATED_SOURCE "${CMAKE_SOURCE_DIR}/${SOURCE}/shaders_embedded.c")

add_custom_command(
    OUTPUT ${GENERATED_SOURCE}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/shader_source_generator.py
            ${CMAKE_SOURCE_DIR}/${SOURCE}        # input dir
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

if (${SOURCE} STREQUAL "src_old")

add_executable(game 
    ${SOURCE}/main.c 
    ${SOURCE}/chunks.c 
    ${SOURCE}/baserenderer.c 
    ${SOURCE}/save_file_manager.c 
    ${SOURCE}/string_utilities.c 
    ${SOURCE}/noise_functions.c 
    ${SOURCE}/entities.c
    ${SOURCE}/controls.c
    ${SOURCE}/shaders_embedded.c
    ${SOURCE}/gpu_data_transfer.c
)

endif()

if (${SOURCE} STREQUAL "src_new")

add_executable(game
    ${SOURCE}/main.c
    ${SOURCE}/shaders_embedded.c
    ${SOURCE}/error.c
)

endif()



add_dependencies(game embed_shaders)

target_link_libraries(game minifb GLEW glfw GL m)