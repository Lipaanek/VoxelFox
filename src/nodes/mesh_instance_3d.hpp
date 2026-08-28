#pragma once

#include "../core/renderer/mesh/mesh_manager.hpp"
#include "node3d.hpp"

class MeshInstance3D : public Node3D {
private:
    MeshID mesh = 0;

public:
    MeshInstance3D() {
        this->setName("MeshInstance3D");
    }

    explicit MeshInstance3D(const std::string& name);

    void setMesh(MeshID mesh);
    [[nodiscard]] MeshID getMesh() const;
};
