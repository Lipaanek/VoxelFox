#ifndef VERTEX_HPP
#define VERTEX_HPP
#include <glm/glm.hpp>
#include <functional>

struct Vertex {
    glm::vec3 position{};
    glm::vec3 normal{};
    glm::vec2 uv{};
    glm::vec3 color{1.0f};

    [[nodiscard]] Vertex() = default;
    [[nodiscard]] Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 texCoord, glm::vec3 col = glm::vec3(1.0f))
        : position(pos), normal(norm), uv(texCoord), color(col) {}

    bool operator==(const Vertex& other) const {
        return position == other.position && normal == other.normal && uv == other.uv && color == other.color;
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
            size_t h4 = std::hash<float>{}(v.color.x) ^ 
                        (std::hash<float>{}(v.color.y) << 1) ^ 
                        (std::hash<float>{}(v.color.z) << 2);
            return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
        }
    };
}

#endif