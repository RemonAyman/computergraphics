#include "Shape.h"
#include "DrawingAlgorithms.h"
#include "Transformations.h"
#include <GL/freeglut.h>

void LineShape::draw() {
    if (algo == "DDA") {
        DrawingAlgorithms::drawLineDDA(p1.x, p1.y, p2.x, p2.y, color);
    } else {
        DrawingAlgorithms::drawLineBresenham(p1.x, p1.y, p2.x, p2.y, color);
    }
}

void LineShape::translate(int dx, int dy) {
    p1.x += dx; p1.y += dy;
    p2.x += dx; p2.y += dy;
}

void LineShape::rotate(float angle, Point pivot) {
    Transformations::rotatePoint(p1.x, p1.y, pivot.x, pivot.y, angle);
    Transformations::rotatePoint(p2.x, p2.y, pivot.x, pivot.y, angle);
}

void LineShape::scale(float sx, float sy, Point pivot) {
    // Basic scaling logic
    p1.x = pivot.x + (int)((p1.x - pivot.x) * sx);
    p1.y = pivot.y + (int)((p1.y - pivot.y) * sy);
    p2.x = pivot.x + (int)((p2.x - pivot.x) * sx);
    p2.y = pivot.y + (int)((p2.y - pivot.y) * sy);
}

void CircleShape::draw() {
    if (algo == "Midpoint") {
        DrawingAlgorithms::drawCircleMidpoint(center.x, center.y, radius, color);
    } else {
        DrawingAlgorithms::drawCircleBresenham(center.x, center.y, radius, color);
    }
}

void CircleShape::translate(int dx, int dy) {
    center.x += dx; center.y += dy;
}

void CircleShape::rotate(float angle, Point pivot) {
    Transformations::rotatePoint(center.x, center.y, pivot.x, pivot.y, angle);
}

void CircleShape::scale(float sx, float sy, Point pivot) {
    center.x = pivot.x + (int)((center.x - pivot.x) * sx);
    center.y = pivot.y + (int)((center.y - pivot.y) * sy);
    radius = (int)(radius * sx);
}
