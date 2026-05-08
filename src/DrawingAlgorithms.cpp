#include "DrawingAlgorithms.h"
#include <GL/freeglut.h>
#include <cmath>
#include <algorithm>

PerformanceStats DrawingAlgorithms::lastStats = {"None", 0, 0};

void DrawingAlgorithms::putPixel(int x, int y, Color color) {
    glColor3f(color.r, color.g, color.b);
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
    lastStats.pixelCount++;
}

void DrawingAlgorithms::drawLineDDA(int x1, int y1, int x2, int y2, Color color) {
    auto start = std::chrono::high_resolution_clock::now();
    lastStats.pixelCount = 0;
    lastStats.algoName = "DDA Line";

    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = std::max(std::abs(dx), std::abs(dy));

    float xInc = dx / (float)steps;
    float yInc = dy / (float)steps;

    float x = x1;
    float y = y1;

    for (int i = 0; i <= steps; i++) {
        putPixel(std::round(x), std::round(y), color);
        x += xInc;
        y += yInc;
    }

    auto end = std::chrono::high_resolution_clock::now();
    lastStats.executionTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

void DrawingAlgorithms::drawLineBresenham(int x1, int y1, int x2, int y2, Color color) {
    auto start = std::chrono::high_resolution_clock::now();
    lastStats.pixelCount = 0;
    lastStats.algoName = "Bresenham Line";

    int x = x1, y = y1;
    int dx = std::abs(x2 - x1), dy = std::abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        putPixel(x, y, color);
        if (x == x2 && y == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x += sx; }
        if (e2 < dx) { err += dx; y += sy; }
    }

    auto end = std::chrono::high_resolution_clock::now();
    lastStats.executionTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

void DrawingAlgorithms::drawCircleMidpoint(int xc, int yc, int r, Color color) {
    auto start = std::chrono::high_resolution_clock::now();
    lastStats.pixelCount = 0;
    lastStats.algoName = "Midpoint Circle";

    int x = 0, y = r;
    int p = 1 - r;

    auto plotCircle = [&](int x, int y) {
        putPixel(xc + x, yc + y, color); putPixel(xc - x, yc + y, color);
        putPixel(xc + x, yc - y, color); putPixel(xc - x, yc - y, color);
        putPixel(xc + y, yc + x, color); putPixel(xc - y, yc + x, color);
        putPixel(xc + y, yc - x, color); putPixel(xc - y, yc - x, color);
    };

    plotCircle(x, y);
    while (x < y) {
        x++;
        if (p < 0) p += 2 * x + 1;
        else { y--; p += 2 * (x - y) + 1; }
        plotCircle(x, y);
    }

    auto end = std::chrono::high_resolution_clock::now();
    lastStats.executionTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

void DrawingAlgorithms::drawCircleBresenham(int xc, int yc, int r, Color color) {
    auto start = std::chrono::high_resolution_clock::now();
    lastStats.pixelCount = 0;
    lastStats.algoName = "Bresenham Circle";

    int x = 0, y = r;
    int d = 3 - 2 * r;

    auto plotCircle = [&](int x, int y) {
        putPixel(xc + x, yc + y, color); putPixel(xc - x, yc + y, color);
        putPixel(xc + x, yc - y, color); putPixel(xc - x, yc - y, color);
        putPixel(xc + y, yc + x, color); putPixel(xc - y, yc + x, color);
        putPixel(xc + y, yc - x, color); putPixel(xc - y, yc - x, color);
    };

    plotCircle(x, y);
    while (y >= x) {
        x++;
        if (d > 0) { y--; d = d + 4 * (x - y) + 10; }
        else d = d + 4 * x + 6;
        plotCircle(x, y);
    }

    auto end = std::chrono::high_resolution_clock::now();
    lastStats.executionTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}
