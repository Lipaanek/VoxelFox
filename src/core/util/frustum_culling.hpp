#pragma once
#include <cmath>

#include "aabb.hpp"
#include "../camera/camera.hpp"
#include <glm/glm.hpp>

#include "bounding_sphere.hpp"
#include "../../nodes/transform3d.hpp"
#include <algorithm>

struct Plane {
    glm::vec3 normal;
    float distance;

    float distanceToPlane(const glm::vec3& point) const {
        return glm::dot(this->normal, point) + this->distance;
    }
};

struct Frustum {
    Plane planes[6];
};

inline Frustum createFrustumFromCamera(
    const Camera& cam,
    float aspect
) {
    const glm::mat4 projection =
        cam.getProjectionMatrix(aspect);

    const glm::mat4 view =
        cam.getViewMatrix();

    const glm::mat4 clip =
        projection * view;

    Frustum frustum;

    // Left
    frustum.planes[0].normal = glm::vec3(
        clip[0][3] + clip[0][0],
        clip[1][3] + clip[1][0],
        clip[2][3] + clip[2][0]
    );
    frustum.planes[0].distance =
        clip[3][3] + clip[3][0];

    // Right
    frustum.planes[1].normal = glm::vec3(
        clip[0][3] - clip[0][0],
        clip[1][3] - clip[1][0],
        clip[2][3] - clip[2][0]
    );
    frustum.planes[1].distance =
        clip[3][3] - clip[3][0];

    // Bottom
    frustum.planes[2].normal = glm::vec3(
        clip[0][3] + clip[0][1],
        clip[1][3] + clip[1][1],
        clip[2][3] + clip[2][1]
    );
    frustum.planes[2].distance =
        clip[3][3] + clip[3][1];

    // Top
    frustum.planes[3].normal = glm::vec3(
        clip[0][3] - clip[0][1],
        clip[1][3] - clip[1][1],
        clip[2][3] - clip[2][1]
    );
    frustum.planes[3].distance =
        clip[3][3] - clip[3][1];

    // Near
    frustum.planes[4].normal = glm::vec3(
        clip[0][3] + clip[0][2],
        clip[1][3] + clip[1][2],
        clip[2][3] + clip[2][2]
    );
    frustum.planes[4].distance =
        clip[3][3] + clip[3][2];

    // Far
    frustum.planes[5].normal = glm::vec3(
        clip[0][3] - clip[0][2],
        clip[1][3] - clip[1][2],
        clip[2][3] - clip[2][2]
    );
    frustum.planes[5].distance =
        clip[3][3] - clip[3][2];

    for (Plane& plane : frustum.planes)
    {
        const float length =
            glm::length(plane.normal);

        plane.normal /= length;
        plane.distance /= length;
    }

    return frustum;
}

inline bool isOnFrustum(const Frustum& frustum, const glm::mat4& model, const BoundingSphere& sphere) {
    const auto center =
        glm::vec3(model * glm::vec4(sphere.center, 1.0f));

    const float scaleX = glm::length(glm::vec3(model[0]));
    const float scaleY = glm::length(glm::vec3(model[1]));
    const float scaleZ = glm::length(glm::vec3(model[2]));

    const float radius =
        sphere.radius *
        std::max(scaleX, std::max(scaleY, scaleZ));

    for (const Plane& plane : frustum.planes) {
        const float distance =
            plane.distanceToPlane(center);

        if (distance < -radius)
            return false;
    }

    return true;
}

