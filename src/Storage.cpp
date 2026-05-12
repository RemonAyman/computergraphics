#include "Storage.h"
#include "Shape.h"
#include <iostream>
#include <sstream>

void SceneStorage::saveShapes(const std::vector<std::unique_ptr<Shape>>& shapes, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open " << filename << " for saving!" << std::endl;
        return;
    }
    for (const auto& s : shapes) {
        file << s->serialize() << "\n";
    }
    file.close();
    std::cout << "Scene saved to: " << filename << std::endl;
}

std::vector<std::unique_ptr<Shape>> SceneStorage::loadShapes(const std::string& filename) {
    std::vector<std::unique_ptr<Shape>> shapes;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open " << filename << " for loading!" << std::endl;
        return shapes;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string type;
        ss >> type;
        if (type == "LINE") {
            int x1, y1, x2, y2;
            float r, g, b;
            std::string algo;
            ss >> x1 >> y1 >> x2 >> y2 >> r >> g >> b >> algo;
            shapes.push_back(std::make_unique<LineShape>(Point(x1, y1), Point(x2, y2), Color(r, g, b), algo));
        } else if (type == "CIRCLE") {
            int xc, yc, rad;
            float r, g, b;
            std::string algo;
            ss >> xc >> yc >> rad >> r >> g >> b >> algo;
            shapes.push_back(std::make_unique<CircleShape>(Point(xc, yc), rad, Color(r, g, b), algo));
        } else if (type == "CURVE") {
            int ptCount;
            ss >> ptCount;
            std::vector<Point> pts;
            for (int i = 0; i < ptCount; i++) {
                int px, py;
                ss >> px >> py;
                pts.push_back(Point(px, py));
            }
            float r, g, b;
            std::string algo;
            ss >> r >> g >> b >> algo;
            shapes.push_back(std::make_unique<BezierShape>(pts, Color(r, g, b), algo));
        } else if (type == "CLIP") {
            int x1, y1, x2, y2;
            ss >> x1 >> y1 >> x2 >> y2;
            shapes.push_back(std::make_unique<ClippingRect>(Point(x1, y1), Point(x2, y2), Color(0, 1, 0)));
        } else if (type == "POLYGON") {
            int vCount;
            ss >> vCount;
            std::vector<Point> verts;
            for (int i = 0; i < vCount; i++) {
                int vx, vy;
                ss >> vx >> vy;
                verts.push_back(Point(vx, vy));
            }
            float r, g, b;
            ss >> r >> g >> b;
            shapes.push_back(std::make_unique<PolygonShape>(verts, Color(r, g, b)));
        }
    }
    file.close();
    std::cout << "Loaded " << shapes.size() << " shapes from " << filename << std::endl;
    return shapes;
}
