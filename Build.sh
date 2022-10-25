#!/bin/sh

if [ ! -d "External/eigen" ]
then
    git clone https://gitlab.com/libeigen/eigen.git External/eigen
fi

if [ ! -d "External/stb" ]
then
    git clone https://github.com/nothings/stb.git External/stb
fi

if [ ! -d "External/imgui" ]
then
    git clone https://github.com/ocornut/imgui.git External/imgui
fi

if [ ! -d "External/implot" ]
then
    git clone https://github.com/epezent/implot.git External/implot
fi

if [ ! -d "External/gl3w" ]
then
    git clone https://github.com/skaslev/gl3w.git External/gl3w

    cd External/gl3w
    cmake .
    make
    cd ../..
fi

if [ ! -d "External/glfw" ]
then
    git clone https://github.com/glfw/glfw.git External/glfw



fi

if [ ! -d "Build" ]
then
    mkdir Build
fi

cd Build
cmake ..
make
