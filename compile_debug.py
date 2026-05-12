import subprocess

cmd = [
    r"E:\Program Files\CLion 2025.3.2\bin\mingw\bin\g++.exe",
    "-IE:/graphicsproject/src",
    "-IE:/graphicsproject/cmake-build-debug/_deps/freeglut-src/include",
    "-std=gnu++17",
    "-c", "E:/graphicsproject/src/App.cpp",
    "-o", "App.obj"
]

try:
    result = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    with open("E:/graphicsproject/py_err.txt", "w") as f:
        f.write("STDOUT:\n")
        f.write(result.stdout)
        f.write("\nSTDERR:\n")
        f.write(result.stderr)
        f.write("\nEXITCODE: " + str(result.returncode))
except Exception as e:
    with open("E:/graphicsproject/py_err.txt", "w") as f:
        f.write(str(e))
