#include "Transformations.h"
#include <cmath>

void Transformations::rotatePoint(int& x, int& y, int cx, int cy, float angle) {
    float rad = angle * M_PI / 180.0f;
    int tempX = x - cx;
    int tempY = y - cy;

    x = cx + (int)(tempX * std::cos(rad) - tempY * std::sin(rad));
    y = cy + (int)(tempX * std::sin(rad) + tempY * std::cos(rad));
}

void Transformations::translatePoint(int& x, int& y, int dx, int dy) {
    x += dx;
    y += dy;
}

void Transformations::scalePoint(int& x, int& y, int cx, int cy, float sx, float sy) {
    x = cx + (int)((x - cx) * sx);
    y = cy + (int)((y - cy) * sy);
}

void Transformations::reflectPoint(int& x, int& y, std::string axis) {
    if (axis == "X") y = WINDOW_HEIGHT - y;
    else if (axis == "Y") x = WINDOW_WIDTH - x;
    else if (axis == "Origin") {
        x = WINDOW_WIDTH - x;
        y = WINDOW_HEIGHT - y;
    }
}

void Transformations::shearPoint(int& x, int& y, float shx, float shy) {
    int oldX = x;
    x = x + (int)(shx * y);
    y = y + (int)(shy * oldX);
}
