# CMake setup
cmake_minimum_required(VERSION 3.14)

# Project setup
project(ImageViewer VERSION 1.0)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Our headers
include_directories("Include")

# Setup externals
include_directories("External/eigen")
include_directories("External/stb")
include_directories("External/imgui")
include_directories("External/implot")
include_directories("External/gl3w/include")
include_directories("External/glfw/include")

# Build GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("External/glfw")

# Libs and exe
add_subdirectory(Source)
