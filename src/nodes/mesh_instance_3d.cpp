#include "mesh_instance_3d.hpp"

void MeshInstance3D::setMesh(MeshID mesh) {
    this->mesh = mesh;
}

MeshID MeshInstance3D::getMesh() const {
    return this->mesh;
}

MeshInstance3D::MeshInstance3D(const std::string &name) {
    
}
