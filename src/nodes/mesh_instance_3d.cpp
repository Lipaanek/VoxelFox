#include "mesh_instance_3d.hpp"

MeshInstance3D::MeshInstance3D() {
    this->setName("MeshInstance3D");
}

void MeshInstance3D::setMesh(MeshID mesh, AABB aabb) {
    this->mesh = mesh;

    BoundingSphere sphere {};
    sphere.center =
        (aabb.min + aabb.max) * 0.5f;

    sphere.radius =
        glm::length(aabb.max - sphere.center);

    this->sphere = sphere;
}

BoundingSphere MeshInstance3D::getBoundingSphere() const {
    return this->sphere;
}

MeshID MeshInstance3D::getMesh() const {
    return this->mesh;
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
