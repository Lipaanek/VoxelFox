#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <glm/glm.hpp>

struct Transform {
    glm::vec3 position{0.0f};
    glm::vec3 rotation{0.0f};
    glm::vec3 scale{1.0f};
};

#endif