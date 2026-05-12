@echo off
"E:\Program Files\CLion 2025.3.2\bin\mingw\bin\g++.exe" -IE:/graphicsproject/src -std=gnu++17 -c E:/graphicsproject/src/Storage.cpp -o Storage.obj > out_storage.txt 2>&1
type out_storage.txt
