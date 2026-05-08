#ifndef CLIPPING_H
#define CLIPPING_H

#include "Common.h"
#include <vector>

class Clipping {
public:
    // Region codes for Cohen-Sutherland
    static const int INSIDE = 0; // 0000
    static const int LEFT = 1;   // 0001
    static const int RIGHT = 2;  // 0010
    static const int BOTTOM = 4; // 0100
    static const int TOP = 8;    // 1000

    static int computeCode(int x, int y, int xmin, int ymin, int xmax, int ymax);
    static bool cohenSutherlandClip(int& x1, int& y1, int& x2, int& y2, int xmin, int ymin, int xmax, int ymax);
    
    static std::vector<Point> sutherlandHodgmanClip(const std::vector<Point>& poly, int xmin, int ymin, int xmax, int ymax);
};

#endif
