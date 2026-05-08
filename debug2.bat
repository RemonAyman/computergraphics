@echo off
"E:\Program Files\CLion 2025.3.2\bin\mingw\bin\gcc.exe" -c e:/graphicsproject/src/App.cpp -I e:/graphicsproject/src -o e:/graphicsproject/cmake-build-debug/App.obj > err2.txt 2>&1
type err2.txt
