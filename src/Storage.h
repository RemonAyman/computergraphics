#ifndef GRAPHICS_SCENE_STORAGE_H
#define GRAPHICS_SCENE_STORAGE_H

#include "Shape.h"
#include <fstream>
#include <vector>
#include <memory>
#include <string>

class SceneStorage {
public:
    static void saveShapes(const std::vector<std::unique_ptr<Shape>>& shapes, const std::string& filename);
    static std::vector<std::unique_ptr<Shape>> loadShapes(const std::string& filename);
};

#endif
