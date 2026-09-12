#pragma once

#include "../core/renderer/mesh/mesh_manager.hpp"
#include "node3d.hpp"
#include <glm/glm.hpp>

class MeshInstance3D : public Node3D {
private:
    MeshID mesh = static_cast<MeshID>(-1);
    glm::vec3 color { 1.0f };

public:
    MeshInstance3D();
    explicit MeshInstance3D(const std::string& name);

    void setMesh(MeshID mesh);
    [[nodiscard]] MeshID getMesh() const;

    void setColor(glm::vec3 color);
    [[nodiscard]] glm::vec3 getColor() const;
};
