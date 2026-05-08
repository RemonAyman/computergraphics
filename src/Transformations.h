#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

#include "Common.h"

class Transformations {
public:
    static void rotatePoint(int& x, int& y, int cx, int cy, float angle);
    static void translatePoint(int& x, int& y, int dx, int dy);
    static void scalePoint(int& x, int& y, int cx, int cy, float sx, float sy);
    static void reflectPoint(int& x, int& y, std::string axis);
    static void shearPoint(int& x, int& y, float shx, float shy);
};

#endif
