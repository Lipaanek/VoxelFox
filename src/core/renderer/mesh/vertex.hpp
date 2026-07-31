#pragma once

#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position{};
    glm::vec3 color{1.0f};

    Vertex(glm::vec3 position, glm::vec3 color = glm::vec3(1.0f)) : position(position), color(color) {};
};