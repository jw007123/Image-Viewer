# CMake setup
cmake_minimum_required(VERSION 3.14)

# Project setup
project(ImageViewer VERSION 1.0)

# C++17
set(CMAKE_CXX_STANDARD 20)
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
include_directories("External/nativefiledialog/src/include")

link_directories("External/nativefiledialog/build/lib/Release/x64")

# Build GLFW
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_EXAMPLES OFF)

# Libs and exe
add_subdirectory("External/glfw")
add_subdirectory(Source)