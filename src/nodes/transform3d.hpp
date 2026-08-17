#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

struct Transform3D {
    glm::vec3 position {0.0f};
    glm::vec3 scale {1.0f};
    glm::quat rotation {1.0f, 0.0f, 0.0f, 0.0f};

    [[nodiscard]]
    glm::mat4 getMatrix() const {
        const glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
        const glm::mat4 rotationMatrix = glm::mat4_cast(rotation);
        const glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

        return translation * rotationMatrix * scaleMatrix;
    }
};