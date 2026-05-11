#include "Curves.h"
#include <GL/freeglut.h>
#include <vector>
#include <cmath>

void Curves::drawBezier(const std::vector<Point>& points, int numPoints, Color color) {
    if (points.size() < 4) return;
    glColor3f(0.5f, 0.5f, 0.5f);
    glEnable(GL_LINE_STIPPLE); glLineStipple(1, 0xAAAA);
    glBegin(GL_LINE_STRIP);
    for (const auto& p : points) glVertex2i(p.x, p.y);
    glEnd();
    glDisable(GL_LINE_STIPPLE);

    glColor3f(color.r, color.g, color.b);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= numPoints; i++) {
        float t = (float)i / numPoints;
        float x = std::pow(1 - t, 3) * points[0].x + 3 * std::pow(1 - t, 2) * t * points[1].x + 3 * (1 - t) * std::pow(t, 2) * points[2].x + std::pow(t, 3) * points[3].x;
        float y = std::pow(1 - t, 3) * points[0].y + 3 * std::pow(1 - t, 2) * t * points[1].y + 3 * (1 - t) * std::pow(t, 2) * points[2].y + std::pow(t, 3) * points[3].y;
        glVertex2i((int)x, (int)y);
    }
    glEnd();
}

void Curves::drawBSpline(const std::vector<Point>& points, int numPoints, Color color) {
    if (points.size() < 4) return;
    
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINE_STRIP);
    for (const auto& p : points) glVertex2i(p.x, p.y);
    glEnd();

    glColor3f(color.r, color.g, color.b);
    glBegin(GL_LINE_STRIP);
    for (size_t i = 0; i < points.size() - 3; i++) {
        for (int j = 0; j <= numPoints; j++) {
            float t = (float)j / numPoints;
            float t2 = t * t;
            float t3 = t2 * t;

            float b0 = (-t3 + 3 * t2 - 3 * t + 1) / 6.0f;
            float b1 = (3 * t3 - 6 * t2 + 4) / 6.0f;
            float b2 = (-3 * t3 + 3 * t2 + 3 * t + 1) / 6.0f;
            float b3 = t3 / 6.0f;

            float x = b0 * points[i].x + b1 * points[i+1].x + b2 * points[i+2].x + b3 * points[i+3].x;
            float y = b0 * points[i].y + b1 * points[i+1].y + b2 * points[i+2].y + b3 * points[i+3].y;
            glVertex2i((int)x, (int)y);
        }
    }
    glEnd();
}
