#pragma once

#include "node.hpp"
#include "transform3d.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Node3D : public Node {
public:
    Node3D() = default;
    ~Node3D() override = default;

    explicit Node3D(const std::string& name);

    [[nodiscard]] glm::mat4 getLocalMatrix() const;
    [[nodiscard]] glm::mat4 getGlobalMatrix() const;

    void setPosition(const glm::vec3 &position);
    void setScale(const glm::vec3 &scale);
    void setRotation(const glm::quat &rotation);

    [[nodiscard]] glm::vec3 getPosition() const;
    [[nodiscard]] glm::vec3 getScale() const;
    [[nodiscard]] glm::quat getRotation() const;
    [[nodiscard]] Node3D* getParent() const override;

protected:
    Transform3D transform;
};