#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <string>
#include <cmath>
#include <chrono>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct Color {
    float r, g, b;
    Color(float r = 1.0f, float g = 1.0f, float b = 1.0f) : r(r), g(g), b(b) {}
};

struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
};

// Application Constants
const Color APP_COLOR_BG(0.12f, 0.12f, 0.12f);
const Color APP_COLOR_SIDEBAR(0.18f, 0.18f, 0.18f);
const Color APP_COLOR_GRID(0.25f, 0.25f, 0.25f);
const Color APP_COLOR_ACCENT(0.0f, 0.47f, 0.83f);
const Color APP_COLOR_WHITE(1.0f, 1.0f, 1.0f);
const Color APP_COLOR_RED(1.0f, 0.2f, 0.2f);
const Color APP_COLOR_GREEN(0.2f, 1.0f, 0.2f);

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;
const int SIDEBAR_WIDTH = 250;

enum class AppMode {
    NONE,
    DRAW_DDA,
    DRAW_BRESENHAM,
    DRAW_CIRCLE_MIDPOINT,
    DRAW_CIRCLE_BRESENHAM,
    TRANSFORM_TRANSLATE,
    TRANSFORM_ROTATE,
    TRANSFORM_SCALE,
    TRANSFORM_REFLECT,
    TRANSFORM_SHEAR,
    CLIPPING_LINE,
    CLIPPING_POLYGON,
    CURVE_BEZIER,
    CURVE_BSPLINE,
    DRAW_POLYGON
};

struct Button {
    std::string label;
    int x, y, w, h;
    AppMode mode;
    bool isHovered = false;
};

struct PerformanceStats {
    std::string algoName;
    long long executionTime; // microseconds
    int pixelCount;
};

#endif
