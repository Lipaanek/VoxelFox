#pragma once

#include "node3d.hpp"
#include <glm/glm.hpp>

#include "core/resources/resource.hpp"

class MeshInstance3D : public Node3D {
private:
    ResourceID meshID = 0;
    glm::vec3 color { 1.0f };

public:
    MeshInstance3D();
    explicit MeshInstance3D(const std::string& name);

    void setMesh(ResourceID meshID);
    [[nodiscard]] ResourceID getMesh() const;

    void setColor(glm::vec3 color);
    [[nodiscard]] glm::vec3 getColor() const;
};
