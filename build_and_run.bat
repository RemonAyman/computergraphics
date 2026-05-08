@echo off
echo Compiling Computer Graphics Project...
g++ src/*.cpp -o cg_project.exe -lfreeglut -lglew32 -lopengl32 -lglu32
if %errorlevel% neq 0 (
    echo Compilation failed!
    pause
    exit /b %errorlevel%
)
echo Compilation successful! Running project...
cg_project.exe
