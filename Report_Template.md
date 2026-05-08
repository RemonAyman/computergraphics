# Project Report: Fundamental Computer Graphics Algorithms

## 1. Introduction
This project demonstrates the implementation of core 2D computer graphics algorithms from scratch. Instead of using built-in graphics library functions, all rasterization and mathematical transformations are implemented manually to understand the underlying principles of computer graphics.

## 2. Objectives
- Implement DDA and Bresenham line algorithms and compare efficiency.
- Implement Midpoint and Bresenham circle algorithms.
- Apply matrix-based 2D transformations (Rotate, Scale, Translate, Shear).
- Implement interactive clipping and curve generation (Bezier).

## 3. Algorithm Descriptions

### 3.1 Line Drawing (DDA vs Bresenham)
- **DDA**: Uses floating-point increments. Simple but slower due to rounding.
- **Bresenham**: Uses integer decision parameters. Faster and more precise for raster displays.

### 3.2 Circle Drawing
- Uses symmetry properties of a circle to plot 8 points for every calculation.

### 3.3 Transformations
- Implemented using homogenous coordinates and matrix multiplication.

## 4. Performance Analysis
| Algorithm | Pixels Plotted | Time (μs) | Efficiency Notes |
|-----------|----------------|-----------|------------------|
| DDA Line  | [Value]        | [Value]   | Float math       |
| Bresenham | [Value]        | [Value]   | Integer math     |

## 5. Challenges Faced
- Managing library dependencies (FreeGLUT/OpenGL).
- Synchronizing UI coordinates with OpenGL viewport.
- Handling floating-point precision in curve generation.

## 6. Screenshots
![Main Interface]([Path_to_Screenshot])
![Transformations]([Path_to_Screenshot])
![Clipping Demo]([Path_to_Screenshot])

## 7. Conclusion
The project successfully achieves all requirements, providing a robust interactive environment for exploring fundamental graphics concepts.
