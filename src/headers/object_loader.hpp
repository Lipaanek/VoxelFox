#ifndef OBJECT_LOADER_HPP
#define OBJECT_LOADER_HPP

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include "vertex.hpp"

class ObjectLoader {
public:
    ObjectLoader() = default;

    bool load(const std::string& path);
    [[nodiscard]] const std::vector<Vertex>& getVertices() const { return processedVertices_; }
    [[nodiscard]] const std::vector<uint32_t>& getIndices() const { return indices_; }
    [[nodiscard]] bool hasIndices() const { return !indices_.empty(); }
    void clear();

private:
    std::vector<Vertex> vertices_;
    std::vector<glm::vec3> normals_;
    std::vector<Vertex> processedVertices_;
    std::vector<uint32_t> indices_;
    std::unordered_map<Vertex, uint32_t> uniqueVertexMap_;

    void parseVertexLine(const std::string& line);
    void parseNormalLine(const std::string& line);
    void parseFaceLine(const std::string& line);
    uint32_t getOrCreateVertex(const Vertex& v);
};

#endif