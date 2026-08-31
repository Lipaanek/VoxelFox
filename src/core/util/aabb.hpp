#pragma once

#include <glm/glm.hpp>

class Frustum;

struct AABB {
    glm::vec3 min;
    glm::vec3 max;
    bool intersects(const Frustum& frustum) const;
    bool isInside(const Frustum& frustum) const;
};