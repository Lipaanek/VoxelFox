#pragma once

#include "mesh_instance_3d.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <unordered_set>
#include <cstdint>

using VoxelHash = uint64_t;

enum class ModelStatus {
    Baked,
    Unbaked
};

class VoxelModel : public MeshInstance3D {
private:
    float gridSize = 1.0f;
    std::vector<glm::ivec3> voxelPositions;
    std::vector<glm::vec3> voxelColors;
    std::unordered_set<VoxelHash> voxelSet;

    glm::ivec3 snapToGrid(glm::vec3 worldPos) const;
    VoxelHash computeHash(glm::ivec3 pos) const;
    MeshData buildMeshData() const;

    glm::ivec3 worldToGrid(glm::vec3 position) const;
    glm::vec3 gridToWorld(glm::ivec3 position) const;

    ModelStatus status = ModelStatus::Unbaked;

public:
    VoxelModel();
    explicit VoxelModel(const std::string& name, float gridSize);
    ~VoxelModel() override;

    void addVoxel(glm::vec3 worldPos, glm::vec3 color = {1.0f, 1.0f, 1.0f});
    void bake();

    void voxelize(const MeshData& mesh);

    void onTreeEnter(Scene* newScene) override;
};
