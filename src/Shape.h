#ifndef SHAPE_H
#define SHAPE_H

#include "Common.h"
#include <vector>

class Shape {
public:
    enum class Type { LINE, CIRCLE, POLYGON, CURVE, CLIP_RECT };

    Shape(Type type, Color color) : type(type), color(color) {}
    virtual ~Shape() = default;

    virtual void draw() = 0;
    virtual void translate(int dx, int dy) = 0;
    virtual void rotate(float angle, Point pivot) = 0;
    virtual void scale(float sx, float sy, Point pivot) = 0;
    virtual void reflect(std::string axis) = 0;
    virtual void shear(float shx, float shy) = 0;
    virtual std::string serialize() const = 0;

    Type getType() const { return type; }
    Color getColor() const { return color; }

protected:
    Type type;
    Color color;
};

class LineShape : public Shape {
public:
    Point p1, p2;
    std::string algo;

    LineShape(Point p1, Point p2, Color color, std::string algo) 
        : Shape(Type::LINE, color), p1(p1), p2(p2), algo(algo) {}

    void draw() override;
    void translate(int dx, int dy) override;
    void rotate(float angle, Point pivot) override;
    void scale(float sx, float sy, Point pivot) override;
    void reflect(std::string axis) override;
    void shear(float shx, float shy) override;
    std::string serialize() const override;
};

class CircleShape : public Shape {
public:
    Point center;
    int radius;
    std::string algo;

    CircleShape(Point center, int radius, Color color, std::string algo) 
        : Shape(Type::CIRCLE, color), center(center), radius(radius), algo(algo) {}

    void draw() override;
    void translate(int dx, int dy) override;
    void rotate(float angle, Point pivot) override;
    void scale(float sx, float sy, Point pivot) override;
    void reflect(std::string axis) override;
    void shear(float shx, float shy) override;
    std::string serialize() const override;
};

class BezierShape : public Shape {
public:
    std::vector<Point> controlPoints;
    std::string algo;

    BezierShape(const std::vector<Point>& pts, Color color, std::string algo = "Bezier") 
        : Shape(Type::CURVE, color), controlPoints(pts), algo(algo) {}

    void draw() override;
    void translate(int dx, int dy) override;
    void rotate(float angle, Point pivot) override;
    void scale(float sx, float sy, Point pivot) override;
    void reflect(std::string axis) override;
    void shear(float shx, float shy) override;
    std::string serialize() const override;
};

class PolygonShape : public Shape {
public:
    std::vector<Point> vertices;

    PolygonShape(const std::vector<Point>& v, Color color) 
        : Shape(Type::POLYGON, color), vertices(v) {}

    void draw() override;
    void translate(int dx, int dy) override;
    void rotate(float angle, Point pivot) override;
    void scale(float sx, float sy, Point pivot) override;
    void reflect(std::string axis) override;
    void shear(float shx, float shy) override;
    std::string serialize() const override;
};

class ClippingRect : public Shape {
public:
    Point pMin, pMax;

    ClippingRect(Point p1, Point p2, Color color) 
        : Shape(Type::CLIP_RECT, color), pMin(p1), pMax(p2) {
        if (pMin.x > pMax.x) std::swap(pMin.x, pMax.x);
        if (pMin.y > pMax.y) std::swap(pMin.y, pMax.y);
    }

    void draw() override;
    void translate(int dx, int dy) override;
    void rotate(float angle, Point pivot) override;
    void scale(float sx, float sy, Point pivot) override;
    void reflect(std::string axis) override {}
    void shear(float shx, float shy) override {}
    std::string serialize() const override;
};

#endif
