#ifndef SHAPE_H
#define SHAPE_H

#include "Common.h"
#include <vector>

class Shape {
public:
    enum class Type { LINE, CIRCLE, POLYGON, CURVE };

    Shape(Type type, Color color) : type(type), color(color) {}
    virtual ~Shape() = default;

    virtual void draw() = 0;
    virtual void translate(int dx, int dy) = 0;
    virtual void rotate(float angle, Point pivot) = 0;
    virtual void scale(float sx, float sy, Point pivot) = 0;

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
};

#endif
