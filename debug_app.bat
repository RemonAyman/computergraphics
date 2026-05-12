@echo off
"E:\Program Files\CLion 2025.3.2\bin\mingw\bin\g++.exe" -IE:/graphicsproject/src -std=gnu++17 -c E:/graphicsproject/src/App.cpp -o App.obj 2> errors_app.txt
type errors_app.txt
