#ifndef CURVES_H
#define CURVES_H

#include "Common.h"
#include <vector>

class Curves {
public:
    static void drawBezier(const std::vector<Point>& controlPoints, int segments, Color color);
    static void drawBSpline(const std::vector<Point>& controlPoints, int segments, Color color);

private:
    static long long factorial(int n);
    static float nCr(int n, int r);
};

#endif
