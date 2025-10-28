cmake_minimum_required(VERSION 3.20)

set(T c_gwr)

project(${T})

find_package(OpenGL REQUIRED)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)
set(EXAMPLES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/examples)

set(GLFW_DIR ${EXTERNAL_DIR}/glfw-3.4)
set(GLAD_DIR ${EXTERNAL_DIR}/glad)
set(CGLM_DIR ${EXTERNAL_DIR}/cglm-0.9.6)
set(STB_IMAGE_DIR ${EXTERNAL_DIR}/stb_image)

add_subdirectory(${GLFW_DIR} ${CMAKE_BINARY_DIR}/_deps/glfw-build EXCLUDE_FROM_ALL)
add_subdirectory(${CGLM_DIR} ${CMAKE_BINARY_DIR}/_deps/cglm-build EXCLUDE_FROM_ALL)

add_library(glad STATIC ${GLAD_DIR}/src/glad.c)
target_include_directories(glad PUBLIC ${GLAD_DIR}/include)

add_library(${T} STATIC
        src/gwr_window.c
        src/gwr_shader.c
        src/gwr_vertex_buffer.c
        src/gwr_vertex_array.c
        src/gwr_texture.c
        src/gwr_info.c
        src/gwr_log.c
        src/gwr_math.c
)

target_compile_definitions(${T} PRIVATE GLFW_INCLUDE_NONE)
target_include_directories(
        ${T}
        PUBLIC
        include
        PRIVATE
        ${STB_IMAGE_DIR}
)
target_link_libraries(${T} PUBLIC glad glfw OpenGL::GL cglm)
target_compile_options(${T} PRIVATE -Wall -Wextra -Wpedantic)

add_subdirectory(${EXAMPLES_DIR})
