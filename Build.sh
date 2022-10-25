#!/bin/sh
rm -rf External

git clone https://gitlab.com/libeigen/eigen.git External/eigen
git clone https://github.com/nothings/stb.git External/stb
git clone https://github.com/ocornut/imgui.git External/imgui
git clone https://github.com/epezent/implot.git External/implot
git clone https://github.com/skaslev/gl3w.git External/gl3w
git clone https://github.com/glfw/glfw.git External/glfw

mkdir Build
cd Build
cmake ..
make