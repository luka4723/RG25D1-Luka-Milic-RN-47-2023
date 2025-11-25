cmake_minimum_required(VERSION 3.10)
project(2d-grafika-projekat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- GLAD ----
add_library(glad STATIC libs/glad/src/glad.c)
target_include_directories(glad PUBLIC libs/glad/include)

# ---- GLFW ----
add_subdirectory(libs/glfw-3.4/glfw-3.4)

# ---- Dodavanje CPP fajlova ručno ----
add_executable(2d-grafika-projekat
        main.cpp
        Cpp/shader.cpp
        Cpp/VAO.cpp
        Cpp/VBO.cpp
        Cpp/EBO.cpp
        Cpp/helper.cpp
        Cpp/texture.cpp
        Cpp/camera.cpp
        Cpp/stb_image.cpp
        Headers/object.hpp
        Headers/object.hpp
        Cpp/object.cpp
        Cpp/object.cpp
)

# ---- Include directories ----
target_include_directories(2d-grafika-projekat PUBLIC
        ${CMAKE_SOURCE_DIR}/libs/glad/include
        ${CMAKE_SOURCE_DIR}/libs/glfw-3.4/glfw-3.4/include
        ${CMAKE_SOURCE_DIR}/libs/glm
        ${CMAKE_SOURCE_DIR}/Headers
)

# ---- Link libraries ----
target_link_libraries(2d-grafika-projekat
        glad
        glfw
)

# ---- Kopiranje resursa i shader-a kao custom target ----
add_custom_target(copy_resources ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Resources
        ${CMAKE_BINARY_DIR}/Resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/GLSLs
        ${CMAKE_BINARY_DIR}/GLSLs
)

# ---- Neka executable zavisi od copy_resources ----
add_dependencies(2d-grafika-projekat copy_resources)
