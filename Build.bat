@echo off

if not exist Build (
	mkdir Build
)

cd Build
cmake ..
msbuild ImageViewer.sln
cd ..