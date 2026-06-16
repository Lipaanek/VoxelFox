#pragma once

#include "node.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Node3D : public Node {
public:
    glm::vec3 position{0.0f};
    glm::vec3 eulerRotation{0.0f};
    glm::vec3 scale{1.0f};

    void SetPosition(const glm::vec3& pos);
    void Translate(const glm::vec3& delta);
    void SetScale(const glm::vec3& scl);
    void Rotate(const glm::vec3& axis, float angleDeg);
    void LookAt(const glm::vec3& target);

    glm::quat GetQuaternion() const;
    void SetQuaternion(const glm::quat& q);

    glm::mat4 GetLocalTransform() const;
    glm::mat4 GetGlobalTransform() const;

    void BuildUI() override;

    std::string GetNodeType() const override { return "Node3D"; }

private:
    mutable bool transformDirty = true;
    mutable glm::mat4 localTransform{1.0f};
    void UpdateTransform() const;
};
