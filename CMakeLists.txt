cmake_minimum_required(VERSION 3.26.0)
project(OpenGL_GLFW_Sample)
cmake_policy(SET CMP0072 NEW)

find_package(OpenGL REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/vendor)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/lib-vc2022)

file(GLOB_RECURSE IMGUI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/vendor/imgui/*.cpp)

add_executable(OpenGL_GLFW_Sample 
    src/glad.c
    src/Renderer.cpp
    src/VertexBuffer.cpp
    src/IndexBuffer.cpp
    src/VertexArray.cpp
    src/Shader.cpp
    src/Texture.cpp
    src/vendor/stb_image/stb_image.cpp
    src/Cube.cpp
    ${IMGUI_SOURCES}
    src/main.cpp
        src/Engine.cpp
        src/Engine.h
        src/Window.cpp
        src/Window.h
)

set_target_properties(OpenGL_GLFW_Sample PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_compile_definitions(OpenGL_GLFW_Sample PRIVATE PROJECT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(OpenGL_GLFW_Sample PRIVATE
    ${OPENGL_LIBRARIES}
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/lib-vc2022/glfw3.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/lib-vc2022/glfw3_mt.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/lib-vc2022/glfw3dll.lib
)

message(STATUS "Current CMake source directory: ${CMAKE_CURRENT_SOURCE_DIR}")
