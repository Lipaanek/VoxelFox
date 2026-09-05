#include "aabb.hpp"
#include "frustum_culling.hpp"

bool AABB::intersects(const Frustum& frustum) const {
    for (const Plane& plane : frustum.planes) {
        glm::vec3 p = min;
        glm::vec3 n = max;

        if (plane.normal.x >= 0.0f) std::swap(p.x, n.x);
        if (plane.normal.y >= 0.0f) std::swap(p.y, n.y);
        if (plane.normal.z >= 0.0f) std::swap(p.z, n.z);

        if (plane.distanceToPlane(p) < 0.0f) return false;
    }
    return true;
}

bool AABB::isInside(const Frustum& frustum) const {
    for (const Plane& plane : frustum.planes) {
        glm::vec3 n = max;

        if (plane.normal.x >= 0.0f) n.x = min.x;
        if (plane.normal.y >= 0.0f) n.y = min.y;
        if (plane.normal.z >= 0.0f) n.z = min.z;

        if (plane.distanceToPlane(n) < 0.0f) return false;
    }
    return true;
}
