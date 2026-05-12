# 🎓 Graphics Algorithms Engine - Professional V2.2 (Final)

## 📌 Overview
This project is a high-performance, comprehensive implementation of fundamental 2D Computer Graphics algorithms using **C++** and **OpenGL (FreeGLUT)**. Designed as an academic benchmark tool, it provides an interactive GUI to draw, transform, clip, and analyze various algorithms manually implemented from scratch.

---

## 🛠️ Prerequisites & Setup

To run this project, ensure you have the following environment:

### 1. System Requirements
*   **IDE**: CLion 2024/2025 (Highly Recommended).
*   **Compiler**: MinGW-w64 (GCC 13.1+).
*   **Build System**: CMake 3.20+.
*   **Libraries**: OpenGL, FreeGLUT (Handled via CMake).

### 2. Installation & Running (CLion)
1.  **Clone/Open**: Open the project folder in CLion.
2.  **CMake Setup**: CLion will automatically parse `CMakeLists.txt`.
3.  **Reload Project**: If prompted, click "Reload CMake Project".
4.  **Run**: Press the **Run** button.
    *   *Note: All necessary DLLs and resources are managed automatically by the build script.*

---

## 🚀 Advanced Features (V2.2)

### 1. Scientific Benchmarking Charts
*   **Real-time Analysis**: Toggle **Compare & Charts** to see a dedicated side-panel with bar graphs.
*   **Performance Metrics**: Compares execution time (μs) for similar algorithms (e.g., Bresenham vs Midpoint).
*   **Visual Proof**: Provides empirical evidence of algorithm efficiency directly in the UI.

### 2. Mathematical Equation Visualization
*   **Live Equations**: Toggle **Show Equations** to see the math behind every shape on the canvas.
*   **Dynamic Updating**: Equations update in real-time as you translate, rotate, or scale shapes.

### 3. Smooth Animation System
*   **Transitions**: Transformations (Scaling/Rotation) are now interpolated over time for a smooth, high-end feel.
*   **Interactive Control**: Use the **Animate (R)** button to trigger dynamic motion transitions.

### 4. Smart Persistence (Versioned Saves)
*   **Timestamped Files**: Every "Save Scene" creates a unique file (e.g., `scene_20260512_0100.txt`) to prevent overwriting your work.
*   **Console Loading**: Load specific scenes by entering the filename in the console.

---

## 🏛️ Implemented Core Algorithms

| Category | Algorithms |
| :--- | :--- |
| **Line Drawing** | DDA, Bresenham's (Integer-based) |
| **Circle Drawing** | Midpoint Circle, Bresenham's Circle |
| **Curves** | Cubic Bezier, Cubic B-Splines |
| **Transformations** | Translation, Rotation, Scaling, Reflection (X/Y/O), Shearing |
| **Clipping** | Cohen-Sutherland (Lines), Sutherland-Hodgman (Polygons) |

---

## 📝 Documentation & Reports
For detailed technical breakdowns and comparative studies (in Arabic), refer to:
*   [**Technical Explanation (AR)**](file:///e:/graphicsproject/Algorithms_Explanation_AR.md) - How every algorithm works mathematically.
*   [**Comparative Analysis (AR)**](file:///e:/graphicsproject/Algorithms_Comparison_AR.md) - Detailed differences between similar algorithms.

---

## 🎮 UI Controls Guide
*   **Left Click**: Place points / Draw shapes.
*   **Right Click**: (Contextual) Close polygon / Reset.
*   **Keys**: 
    *   `Z`: Undo last action.
    *   `R`: Toggle Global Animation.
    *   `Esc`: Exit Application.

---
**Developed for the Computer Graphics Final Project. © 2026**
