#pragma once

#include "node3d.hpp"
#include <string>
#include <vector>

struct LoadedMesh;
class MeshManager;
struct VoxelChunk;
class Mesh;

class MeshInstance3D : public Node3D {
public:
    std::string meshAssetPath;
    bool selected = false;

    void SetMesh(const std::string& assetPath, MeshManager& meshManager);
    const std::string& GetMesh() const { return meshAssetPath; }

    const Mesh* GetRenderMesh() const;
    glm::vec3 GetBBoxMin() const;
    glm::vec3 GetBBoxMax() const;
    const std::vector<VoxelChunk>* GetVoxelChunks() const;

    void BuildUI() override;

    std::string GetNodeType() const override { return "MeshInstance3D"; }

private:
    const LoadedMesh* meshData_ = nullptr;
};
