#pragma once

#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position{};
    glm::vec3 normal{0.0f, 1.0f, 0.0f};
    glm::vec2 texCoord{0.0f};
    glm::vec3 color{1.0f};

    Vertex(glm::vec3 position,
           glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f),
           glm::vec2 texCoord = glm::vec2(0.0f),
           glm::vec3 color = glm::vec3(1.0f))
        : position(position), normal(normal), texCoord(texCoord), color(color) {};
};