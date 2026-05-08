#ifndef STORAGE_H
#define STORAGE_H

#include "App.h"
#include <fstream>
#include <vector>

class Storage {
public:
    static void save(const std::string& filename, const std::vector<std::unique_ptr<Shape>>& shapes);
    static void load(const std::string& filename, std::vector<std::unique_ptr<Shape>>& shapes);
};

#endif
