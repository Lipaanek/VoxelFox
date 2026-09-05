#pragma once

#include "../core/renderer/mesh/mesh_manager.hpp"
#include "node3d.hpp"
#include <glm/glm.hpp>

#include "core/util/aabb.hpp"
#include "core/util/bounding_sphere.hpp"

class MeshInstance3D : public Node3D {
private:
    MeshID mesh = static_cast<MeshID>(-1);
    glm::vec3 color { 1.0f };
    BoundingSphere sphere {};
    mutable BoundingSphere m_globalSphere {};

public:
    MeshInstance3D();
    explicit MeshInstance3D(const std::string& name);

    [[nodiscard]] BoundingSphere getBoundingSphere() const;
    [[nodiscard]] BoundingSphere getGlobalBoundingSphere() const;

    void setMesh(MeshID mesh, const AABB &aabb);
    [[nodiscard]] MeshID getMesh() const;

    void setColor(glm::vec3 color);
    [[nodiscard]] glm::vec3 getColor() const;

    void onTreeEnter(Scene *newScene) override;
};
