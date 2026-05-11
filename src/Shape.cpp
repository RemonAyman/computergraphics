#include "Shape.h"
#include "DrawingAlgorithms.h"
#include "Transformations.h"
#include "Curves.h"
#include <GL/freeglut.h>

void LineShape::draw() {
    if (algo == "DDA") DrawingAlgorithms::drawLineDDA(p1.x, p1.y, p2.x, p2.y, color);
    else DrawingAlgorithms::drawLineBresenham(p1.x, p1.y, p2.x, p2.y, color);
}

void LineShape::translate(int dx, int dy) { p1.x += dx; p1.y += dy; p2.x += dx; p2.y += dy; }
void LineShape::rotate(float angle, Point pivot) { Transformations::rotatePoint(p1.x, p1.y, pivot.x, pivot.y, angle); Transformations::rotatePoint(p2.x, p2.y, pivot.x, pivot.y, angle); }
void LineShape::scale(float sx, float sy, Point pivot) {
    p1.x = pivot.x + (int)((p1.x - pivot.x) * sx); p1.y = pivot.y + (int)((p1.y - pivot.y) * sy);
    p2.x = pivot.x + (int)((p2.x - pivot.x) * sx); p2.y = pivot.y + (int)((p2.y - pivot.y) * sy);
}
void LineShape::reflect(std::string axis) { Transformations::reflectPoint(p1.x, p1.y, axis); Transformations::reflectPoint(p2.x, p2.y, axis); }
void LineShape::shear(float shx, float shy) { Transformations::shearPoint(p1.x, p1.y, shx, shy); Transformations::shearPoint(p2.x, p2.y, shx, shy); }
std::string LineShape::serialize() const { return "LINE " + std::to_string(p1.x) + " " + std::to_string(p1.y) + " " + std::to_string(p2.x) + " " + std::to_string(p2.y) + " " + std::to_string(color.r) + " " + std::to_string(color.g) + " " + std::to_string(color.b) + " " + algo; }

void CircleShape::draw() {
    if (algo == "Midpoint") DrawingAlgorithms::drawCircleMidpoint(center.x, center.y, radius, color);
    else DrawingAlgorithms::drawCircleBresenham(center.x, center.y, radius, color);
}

void CircleShape::translate(int dx, int dy) { center.x += dx; center.y += dy; }
void CircleShape::rotate(float angle, Point pivot) { Transformations::rotatePoint(center.x, center.y, pivot.x, pivot.y, angle); }
void CircleShape::scale(float sx, float sy, Point pivot) {
    center.x = pivot.x + (int)((center.x - pivot.x) * sx); center.y = pivot.y + (int)((center.y - pivot.y) * sy);
    radius = (int)(radius * sx);
}
void CircleShape::reflect(std::string axis) { Transformations::reflectPoint(center.x, center.y, axis); }
void CircleShape::shear(float shx, float shy) { Transformations::shearPoint(center.x, center.y, shx, shy); }
std::string CircleShape::serialize() const { return "CIRCLE " + std::to_string(center.x) + " " + std::to_string(center.y) + " " + std::to_string(radius) + " " + std::to_string(color.r) + " " + std::to_string(color.g) + " " + std::to_string(color.b) + " " + algo; }

// Bezier Implementation
void BezierShape::draw() { Curves::drawBezier(controlPoints, 50, color); }
void BezierShape::translate(int dx, int dy) { for (auto& p : controlPoints) { p.x += dx; p.y += dy; } }
void BezierShape::rotate(float angle, Point pivot) { for (auto& p : controlPoints) Transformations::rotatePoint(p.x, p.y, pivot.x, pivot.y, angle); }
void BezierShape::scale(float sx, float sy, Point pivot) { for (auto& p : controlPoints) { p.x = pivot.x + (int)((p.x - pivot.x) * sx); p.y = pivot.y + (int)((p.y - pivot.y) * sy); } }
void BezierShape::reflect(std::string axis) { for (auto& p : controlPoints) Transformations::reflectPoint(p.x, p.y, axis); }
void BezierShape::shear(float shx, float shy) { for (auto& p : controlPoints) Transformations::shearPoint(p.x, p.y, shx, shy); }
std::string BezierShape::serialize() const {
    std::string s = "CURVE " + std::to_string(controlPoints.size());
    for (auto& p : controlPoints) s += " " + std::to_string(p.x) + " " + std::to_string(p.y);
    s += " " + std::to_string(color.r) + " " + std::to_string(color.g) + " " + std::to_string(color.b);
    return s;
}

// Polygon Implementation
void PolygonShape::draw() {
    if (vertices.empty()) return;
    glColor3f(color.r, color.g, color.b);
    glBegin(GL_LINE_LOOP);
    for (const auto& v : vertices) glVertex2i(v.x, v.y);
    glEnd();
}
void PolygonShape::translate(int dx, int dy) { for (auto& v : vertices) { v.x += dx; v.y += dy; } }
void PolygonShape::rotate(float angle, Point pivot) { for (auto& v : vertices) Transformations::rotatePoint(v.x, v.y, pivot.x, pivot.y, angle); }
void PolygonShape::scale(float sx, float sy, Point pivot) { for (auto& v : vertices) { v.x = pivot.x + (int)((v.x - pivot.x) * sx); v.y = pivot.y + (int)((v.y - pivot.y) * sy); } }
void PolygonShape::reflect(std::string axis) { for (auto& v : vertices) Transformations::reflectPoint(v.x, v.y, axis); }
void PolygonShape::shear(float shx, float shy) { for (auto& v : vertices) Transformations::shearPoint(v.x, v.y, shx, shy); }
std::string PolygonShape::serialize() const {
    std::string s = "POLYGON " + std::to_string(vertices.size());
    for (auto& v : vertices) s += " " + std::to_string(v.x) + " " + std::to_string(v.y);
    s += " " + std::to_string(color.r) + " " + std::to_string(color.g) + " " + std::to_string(color.b);
    return s;
}

// Clipping Rect Implementation
void ClippingRect::draw() {
    glColor3f(0.0f, 1.0f, 0.0f); // Green for clip window
    glEnable(GL_LINE_STIPPLE); glLineStipple(1, 0xF0F0);
    glBegin(GL_LINE_LOOP);
    glVertex2i(pMin.x, pMin.y); glVertex2i(pMax.x, pMin.y);
    glVertex2i(pMax.x, pMax.y); glVertex2i(pMin.x, pMax.y);
    glEnd();
    glDisable(GL_LINE_STIPPLE);
}
void ClippingRect::translate(int dx, int dy) { pMin.x += dx; pMin.y += dy; pMax.x += dx; pMax.y += dy; }
void ClippingRect::rotate(float angle, Point pivot) {} 
void ClippingRect::scale(float sx, float sy, Point pivot) {}
std::string ClippingRect::serialize() const { return "CLIP " + std::to_string(pMin.x) + " " + std::to_string(pMin.y) + " " + std::to_string(pMax.x) + " " + std::to_string(pMax.y); }
