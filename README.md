# 🎓 Graphics Algorithms Engine - Final Project

## 📌 Overview
This project is a comprehensive implementation of fundamental 2D Computer Graphics algorithms using **C++** and **OpenGL (FreeGLUT)**. It provides an interactive GUI to draw, transform, clip, and compare various algorithms manually implemented from scratch (no high-level OpenGL drawing functions used for core algorithms).

---

## 🛠️ Prerequisites & Setup

To run this project, you need to have the following installed:

### 1. Requirements
*   **IDE**: CLion (Recommended) or Visual Studio Code.
*   **Compiler**: MinGW-w64 (GCC 13.1+) or any C++17 compatible compiler.
*   **Build System**: CMake 3.20+.
*   **Graphics Library**: FreeGLUT (included in the `external` folder/CMake config).

### 2. How to Run (CLion)
1.  **Open Project**: Launch CLion and open the project folder.
2.  **Configure CMake**: The project uses a `CMakeLists.txt` file. CLion will automatically detect it. 
3.  **Reload CMake**: If prompted, click "Reload CMake Project".
4.  **Build & Run**: Click the **Run** button (Green Arrow) in the top right.
    *   *Note: The DLLs are automatically copied to the output directory during the build process.*

### 3. Manual Build (Command Line)
If you prefer the command line:
```bash
mkdir build && cd build
cmake ..
cmake --build .
./GraphicsAlgorithms
```

---

## 🚀 Implemented Algorithms

### 1. Line Drawing
*   **DDA (Digital Differential Analyzer)**: Incremental algorithm using floating-point math.
*   **Bresenham's Line Algorithm**: High-performance integer-only algorithm.

### 2. Circle Drawing
*   **Midpoint Circle Algorithm**: Uses 8-way symmetry and decision parameters.
*   **Bresenham's Circle Algorithm**: Optimized error-minimization approach.

### 3. 2D Transformations
*   **Translation**: Moving shapes by (dx, dy).
*   **Rotation**: Rotating shapes around a pivot point.
*   **Scaling**: Uniform scaling relative to a pivot point.
*   **Reflection**: Flip shapes across X-axis, Y-axis, or Origin.
*   **Shearing**: Slanting shapes horizontally/vertically.

### 4. Clipping
*   **Cohen-Sutherland (Line)**: Region-code based clipping for line segments.
*   **Sutherland-Hodgman (Polygon)**: Successive edge clipping for closed polygons.

### 5. Curves
*   **Cubic Bezier Curves**: Smooth curves using 4 control points (Bernstein basis).
*   **Cubic B-Splines**: Local control curves using 4 control points.

---

## 🎮 How to Use the App

*   **Drawing Shapes**: Click a tool (e.g., DDA Line), then click twice on the grid.
*   **Bezier/B-Spline**: Click 4 times on the grid to define control points.
*   **Polygons**: Click multiple times (Left Click) to add vertices, and **Right Click** to close the polygon.
*   **Clipping**:
    1.  Draw a **Clip Window** over your shapes.
    2.  Click **S-H Clip** to clip polygons.
    3.  Lines are clipped automatically when the window is drawn.
*   **Persistence**:
    *   **Save Scene**: Saves current state to `scene.txt`.
    *   **Load Scene**: Restores scene (Auto-loads on startup).
*   **Comparison**: Use **Compare (Split Screen)** to view performance metrics side-by-side.

---

## 🏆 Bonus Features (Fully Implemented)
*   [x] **Real-time Performance Stats**: Shows pixel count and execution time (μs).
*   [x] **Undo/Redo System**: Full support for all shape types (Keys: Z/Y).
*   [x] **Interactive Animation**: Rotate all objects in the scene dynamically (Key: R).
*   [x] **Save/Load System**: Full serialization of the drawing scene.
*   [x] **Sutherland-Hodgman Polygon Clipping**: Advanced clipping for closed shapes.
*   [x] **Modern Dark UI**: Intuitive sidebar with hover effects.

---

## 📝 Documentation
For a detailed technical explanation of each algorithm in Arabic, please refer to:
*   [Algorithms Explanation (Arabic)](file:///e:/graphicsproject/Algorithms_Explanation_AR.md)
*   [Algorithms Comparison (Arabic)](file:///e:/graphicsproject/Algorithms_Comparison_AR.md)

---
**Developed for the Computer Graphics Final Project. © 2026**
