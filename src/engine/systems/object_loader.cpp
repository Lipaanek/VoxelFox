#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "../../headers/object_loader.hpp"
#include "../../headers/vertex.hpp"

using std::string;
using std::vector;

namespace {
    vector<string> split(const string& str) {
        vector<string> tokens;
        std::istringstream iss(str);
        string token;
        while (iss >> token) {
            tokens.push_back(token);
        }
        return tokens;
    }
}

void ObjectLoader::clear() {
    vertices_.clear();
    normals_.clear();
    processedVertices_.clear();
    indices_.clear();
    uniqueVertexMap_.clear();
}

bool ObjectLoader::load(const std::string& path) {
    clear();

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return false;
    }

    string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        if (line[0] == 'v') {
            if (line.size() > 1 && line[1] == 'n') {
                parseNormalLine(line);
            } else {
                parseVertexLine(line);
            }
        } else if (line[0] == 'f') {
            parseFaceLine(line);
        }
    }

    file.close();
    return true;
}

void ObjectLoader::parseVertexLine(const string& line) {
    auto tokens = split(line);
    if (tokens.size() < 4) return;

    float x = std::stof(tokens[1]);
    float y = std::stof(tokens[2]);
    float z = std::stof(tokens[3]);

    vertices_.emplace_back(glm::vec3(x, y, z), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f));
}

void ObjectLoader::parseNormalLine(const string& line) {
    auto tokens = split(line);
    if (tokens.size() < 4) return;

    float x = std::stof(tokens[1]);
    float y = std::stof(tokens[2]);
    float z = std::stof(tokens[3]);

    normals_.emplace_back(x, y, z);
}

uint32_t ObjectLoader::getOrCreateVertex(const Vertex& v) {
    auto it = uniqueVertexMap_.find(v);
    if (it != uniqueVertexMap_.end()) {
        return it->second;
    }

    uint32_t index = static_cast<uint32_t>(processedVertices_.size());
    processedVertices_.push_back(v);
    uniqueVertexMap_[v] = index;
    return index;
}

void ObjectLoader::parseFaceLine(const string& line) {
    auto tokens = split(line);
    int numIndices = static_cast<int>(tokens.size()) - 1;
    if (numIndices < 3) return;

    vector<std::pair<int, int>> faceIndices;
    for (int i = 1; i <= numIndices; i++) {
        const string& token = tokens[i];
        size_t firstSlash = token.find('/');
        if (firstSlash == string::npos) {
            faceIndices.emplace_back(std::stoi(token) - 1, -1);
        } else {
            int vertexIdx = std::stoi(token.substr(0, firstSlash)) - 1;
            int normalIdx = -1;
            size_t secondSlash = token.find('/', firstSlash + 1);
            if (secondSlash != string::npos && secondSlash + 1 < token.size()) {
                normalIdx = std::stoi(token.substr(secondSlash + 1)) - 1;
            }
            faceIndices.emplace_back(vertexIdx, normalIdx);
        }
    }

    glm::vec3 normal(0.0f, 1.0f, 0.0f);
    if (faceIndices[0].second >= 0 &&
        faceIndices[0].second < static_cast<int>(normals_.size())) {
        normal = normals_[faceIndices[0].second];
    }

    for (int i = 1; i < numIndices - 1; i++) {
        int vi1 = faceIndices[0].first;
        int vi2 = faceIndices[i].first;
        int vi3 = faceIndices[i + 1].first;

        if (vi1 < 0 || vi2 < 0 || vi3 < 0) continue;
        if (vi1 >= static_cast<int>(vertices_.size()) ||
            vi2 >= static_cast<int>(vertices_.size()) ||
            vi3 >= static_cast<int>(vertices_.size())) continue;

        Vertex v1 = vertices_[vi1];
        v1.normal = normal;

        Vertex v2 = vertices_[vi2];
        v2.normal = normal;

        Vertex v3 = vertices_[vi3];
        v3.normal = normal;

        indices_.push_back(getOrCreateVertex(v1));
        indices_.push_back(getOrCreateVertex(v2));
        indices_.push_back(getOrCreateVertex(v3));
    }
}