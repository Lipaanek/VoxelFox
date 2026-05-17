#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include "../../include/color_loader.hpp"

static std::vector<std::string> splitLine(const std::string& str) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

std::vector<Material> loadColorFile(std::string path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        printf("Failed to open file: %s\n", path.c_str());
        return {};
    }

    std::vector<Material> materials;
    std::string currentMatName;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        auto tokens = splitLine(line);
        if (tokens.empty()) continue;

        if (tokens[0] == "newmtl" && tokens.size() >= 2) {
            currentMatName = tokens[1];
        } else if (tokens[0] == "Kd" && tokens.size() >= 4 && !currentMatName.empty()) {
            float r = std::stof(tokens[1]);
            float g = std::stof(tokens[2]);
            float b = std::stof(tokens[3]);
            materials.push_back({{r, g, b}, currentMatName});
            currentMatName.clear();
        }
    }

    return materials;
}