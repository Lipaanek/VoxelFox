#pragma once

#include <glm/glm.hpp>

struct Transform2D {
    glm::vec2 position {0.0f };
    glm::vec2 scale { 1.0f };
    glm::vec2 dimensions { 0.0f };
    glm::vec2 anchor { 0.5f };
};