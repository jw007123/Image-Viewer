# CMake setup
cmake_minimum_required(VERSION 3.11)

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

# Build and add GL3W headers
add_subdirectory(External/gl3w)

# Build and link GLFW

# Libs and exe
add_subdirectory(Source)