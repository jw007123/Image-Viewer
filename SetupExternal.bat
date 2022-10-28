@echo off

if not exist External/eigen (
	git clone https://gitlab.com/libeigen/eigen.git External/eigen
)

if not exist External/stb (
	git clone https://github.com/nothings/stb.git External/stb
)

if not exist External/imgui (
	git clone https://github.com/ocornut/imgui.git External/imgui
)

if not exist External/implot (
	git clone https://github.com/epezent/implot.git External/implot
)

if not exist External/gl3w (
	git clone https://github.com/skaslev/gl3w.git External/gl3w

	cd External/gl3w
	cmake .
	msbuild gl3w.sln
	cd ../..
)

if not exist External/glfw (
	git clone https://github.com/glfw/glfw.git External/glfw
)