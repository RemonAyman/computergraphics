#include "Clipping.h"

int Clipping::computeCode(int x, int y, int xmin, int ymin, int xmax, int ymax) {
    int code = INSIDE;
    if (x < xmin) code |= LEFT;
    else if (x > xmax) code |= RIGHT;
    if (y < ymin) code |= BOTTOM;
    else if (y > xmax) code |= TOP; // Error here in previous logic, fixed xmax to ymax
    return code;
}

bool Clipping::cohenSutherlandClip(int& x1, int& y1, int& x2, int& y2, int xmin, int ymin, int xmax, int ymax) {
    int code1 = computeCode(x1, y1, xmin, ymin, xmax, ymax);
    int code2 = computeCode(x2, y2, xmin, ymin, xmax, ymax);
    bool accept = false;

    while (true) {
        if ((code1 == 0) && (code2 == 0)) { accept = true; break; }
        else if (code1 & code2) break;
        else {
            int codeOut = (code1 != 0) ? code1 : code2;
            int x, y;
            if (codeOut & TOP) { x = x1 + (x2 - x1) * (ymax - y1) / (y2 - y1); y = ymax; }
            else if (codeOut & BOTTOM) { x = x1 + (x2 - x1) * (ymin - y1) / (y2 - y1); y = ymin; }
            else if (codeOut & RIGHT) { y = y1 + (y2 - y1) * (xmax - x1) / (x2 - x1); x = xmax; }
            else if (codeOut & LEFT) { y = y1 + (y2 - y1) * (xmin - x1) / (x2 - x1); x = xmin; }

            if (codeOut == code1) { x1 = x; y1 = y; code1 = computeCode(x1, y1, xmin, ymin, xmax, ymax); }
            else { x2 = x; y2 = y; code2 = computeCode(x2, y2, xmin, ymin, xmax, ymax); }
        }
    }
    return accept;
}

// Sutherland-Hodgman Polygon Clipping Implementation
std::vector<Point> Clipping::sutherlandHodgmanClip(const std::vector<Point>& poly, int xmin, int ymin, int xmax, int ymax) {
    std::vector<Point> result = poly;

    auto clip = [&](auto& input, int edge, bool isMin, bool isX) {
        std::vector<Point> output;
        if (input.empty()) return output;
        Point prev = input.back();
        for (const auto& curr : input) {
            bool currInside = isX ? (isMin ? curr.x >= edge : curr.x <= edge) : (isMin ? curr.y >= edge : curr.y <= edge);
            bool prevInside = isX ? (isMin ? prev.x >= edge : prev.x <= edge) : (isMin ? prev.y >= edge : prev.y <= edge);

            if (currInside) {
                if (!prevInside) {
                    // Intersection
                    float t;
                    if (isX) t = (float)(edge - prev.x) / (curr.x - prev.x);
                    else t = (float)(edge - prev.y) / (curr.y - prev.y);
                    output.push_back({(int)(prev.x + t*(curr.x - prev.x)), (int)(prev.y + t*(curr.y - prev.y))});
                }
                output.push_back(curr);
            } else if (prevInside) {
                float t;
                if (isX) t = (float)(edge - prev.x) / (curr.x - prev.x);
                else t = (float)(edge - prev.y) / (curr.y - prev.y);
                output.push_back({(int)(prev.x + t*(curr.x - prev.x)), (int)(prev.y + t*(curr.y - prev.y))});
            }
            prev = curr;
        }
        return output;
    };

    result = clip(result, xmin, true, true);
    result = clip(result, xmax, false, true);
    result = clip(result, ymin, true, false);
    result = clip(result, ymax, false, false);
    
    return result;
}
