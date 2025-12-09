cmake_minimum_required(VERSION 3.10)
project(grass)

set(CMAKE_CXX_STANDARD 17)

# GLFW
find_package(glfw3 3.4 REQUIRED)

# GLAD
add_library(glad src/glad.c)
target_include_directories(glad PUBLIC include)

# GLM
find_package(glm REQUIRED)

# Executable
add_executable(grass
    src/main.cpp
    src/geometry.cpp
)

# Link libraries
target_link_libraries(grass
    PRIVATE
        glad                # local GLAD
        dl                  # needed for glad on linux
        glfw                # system GLFW
        pthread             # glfw may require pthread
        glm::glm
)

# Include directories
target_include_directories(grass PRIVATE include)
