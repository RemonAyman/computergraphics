#include "Curves.h"
#include <GL/freeglut.h>
#include <cmath>

long long Curves::factorial(int n) {
    if (n <= 1) return 1;
    long long res = 1;
    for (int i = 2; i <= n; i++) res *= i;
    return res;
}

float Curves::nCr(int n, int r) {
    return (float)(factorial(n) / (factorial(r) * factorial(n - r)));
}

void Curves::drawBezier(const std::vector<Point>& controlPoints, int segments, Color color) {
    if (controlPoints.size() < 2) return;

    int n = controlPoints.size() - 1;
    glColor3f(color.r, color.g, color.b);
    glBegin(GL_LINE_STRIP);

    for (int i = 0; i <= segments; i++) {
        float t = (float)i / segments;
        float x = 0, y = 0;

        for (int j = 0; j <= n; j++) {
            float blend = nCr(n, j) * std::pow(t, j) * std::pow(1 - t, n - j);
            x += controlPoints[j].x * blend;
            y += controlPoints[j].y * blend;
        }
        glVertex2f(x, y);
    }
    glEnd();

    // Draw control polygon (dashed)
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0xAAAA);
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINE_STRIP);
    for (const auto& p : controlPoints) glVertex2i(p.x, p.y);
    glEnd();
    glDisable(GL_LINE_STIPPLE);
}

void Curves::drawBSpline(const std::vector<Point>& controlPoints, int segments, Color color) {
    // Basic B-Spline implementation (placeholder for full logic)
    if (controlPoints.size() < 4) return;
    drawBezier(controlPoints, segments, color); // Fallback to bezier for demo
}
