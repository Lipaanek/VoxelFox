#include "mesh_instance_3d.hpp"

MeshInstance3D::MeshInstance3D() {
    this->setName("MeshInstance3D");
}

void MeshInstance3D::setMesh(ResourceID meshID) {
    this->meshID = meshID;
}

ResourceID MeshInstance3D::getMesh() const {
    return this->meshID;
}

void MeshInstance3D::setColor(glm::vec3 color) {
    this->color = color;
}

glm::vec3 MeshInstance3D::getColor() const {
    return this->color;
}

MeshInstance3D::MeshInstance3D(const std::string &name) {
    this->setName(name);
}
