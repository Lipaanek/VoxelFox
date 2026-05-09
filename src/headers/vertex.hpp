#ifndef VERTEX_HPP
#define VERTEX_HPP
#include <glm/glm.hpp>
#include <functional>

struct Vertex {
    glm::vec3 position{};
    glm::vec3 normal{};
    glm::vec2 uv{};

    [[nodiscard]] Vertex() = default;
    [[nodiscard]] Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 texCoord)
        : position(pos), normal(norm), uv(texCoord) {}

    bool operator==(const Vertex& other) const {
        return position == other.position && normal == other.normal && uv == other.uv;
    }
};

namespace std {
    template<>
    struct hash<Vertex> {
        size_t operator()(const Vertex& v) const noexcept {
            size_t h1 = std::hash<float>{}(v.position.x) ^ 
                        (std::hash<float>{}(v.position.y) << 1) ^ 
                        (std::hash<float>{}(v.position.z) << 2);
            size_t h2 = std::hash<float>{}(v.normal.x) ^ 
                        (std::hash<float>{}(v.normal.y) << 1) ^ 
                        (std::hash<float>{}(v.normal.z) << 2);
            size_t h3 = std::hash<float>{}(v.uv.x) ^ 
                        (std::hash<float>{}(v.uv.y) << 1);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
}

#endif