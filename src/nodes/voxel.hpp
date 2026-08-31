#pragma once

#include "mesh_instance_3d.hpp"
#include <glm/glm.hpp>
#include "../core/renderer/mesh/mesh_data.hpp"
#include "core/renderer/mesh/mesh_manager.hpp"

class Voxel : public MeshInstance3D {
private:
    float size = 1.0f;
    glm::vec3 color { 1.0f };
    using MeshInstance3D::setMesh;

public:
    Voxel();
    explicit Voxel(const std::string& name);
    ~Voxel() override;

    void onTreeEnter(Scene *newScene) override;
    void onTreeExit(Scene *currentScene) override;

    [[nodiscard]] MeshData buildMeshData() const;

    void setSize(float size);
};