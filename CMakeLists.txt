# CMake setup
cmake_minimum_required(VERSION 3.14)

# Project setup
project(ImageViewer VERSION 1.0)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Our headers
include_directories("Include")

# Header only deps
include_directories("External/eigen")
include_directories("External/stb")
include_directories("External/imgui")
include_directories("External/implot")
include_directories("External/gl3w/include")

# Build and link GLFW

# Libs and exe
add_subdirectory(Source)
