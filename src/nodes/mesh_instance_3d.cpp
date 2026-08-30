#include "mesh_instance_3d.hpp"

MeshInstance3D::MeshInstance3D() {
    this->setName("MeshInstance3D");
}

void MeshInstance3D::setMesh(MeshID mesh) {
    this->mesh = mesh;
}

MeshID MeshInstance3D::getMesh() const {
    return this->mesh;
}

MeshInstance3D::MeshInstance3D(const std::string &name) {
    this->setName(name);
}
