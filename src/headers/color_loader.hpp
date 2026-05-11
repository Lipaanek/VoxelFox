#ifndef COLOR_LOADER_HPP
#define COLOR_LOADER_HPP

#include <vector>
#include <string>

struct Color {
    float r;
    float g;
    float b;
};

struct Material {
    Color color;
    std::string name;
};

std::vector<Material> loadColorFile(std::string path);

#endif