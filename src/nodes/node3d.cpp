#include "node3d.hpp"

Node3D::Node3D() {
    this->setName("Node3D");
}

Node3D::Node3D(const std::string &name) {
    this->name = name;
}

Node3D* Node3D::getParent() const {
    return dynamic_cast<Node3D*>(parent);
}

glm::mat4 Node3D::getGlobalMatrix() const {
    if (const Node3D* parent3D = getParent())
        return parent3D->getGlobalMatrix() * getLocalMatrix();

    return getLocalMatrix();
}

glm::mat4 Node3D::getLocalMatrix() const {
    return this->transform.getMatrix();
}

void Node3D::setPosition(const glm::vec3& position) {
    this->transform.position = position;
}

void Node3D::setScale(const glm::vec3 &scale) {
    this->transform.scale = scale;
}

void Node3D::setRotation(const glm::quat &rotation) {
    this->transform.rotation = rotation;
}

glm::vec3 Node3D::getPosition() const {
    return this->transform.position;
}

glm::quat Node3D::getRotation() const {
    return this->transform.rotation;
}

glm::vec3 Node3D::getScale() const {
    return this->transform.scale;
}
