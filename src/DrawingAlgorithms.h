#ifndef DRAWINGALGORITHMS_H
#define DRAWINGALGORITHMS_H

#include "Common.h"

class DrawingAlgorithms {
public:
    static void drawLineDDA(int x1, int y1, int x2, int y2, Color color);
    static void drawLineBresenham(int x1, int y1, int x2, int y2, Color color);
    static void drawCircleMidpoint(int xc, int yc, int r, Color color);
    static void drawCircleBresenham(int xc, int yc, int r, Color color);

    static PerformanceStats getLastStats() { return lastStats; }

private:
    static void putPixel(int x, int y, Color color);
    static PerformanceStats lastStats;
};

#endif
