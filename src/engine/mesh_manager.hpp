#pragma once

#include <string>
#include <vector>
#include "../include/loaded_mesh.hpp"

class MeshManager {
public:
    MeshManager() = default;
    ~MeshManager() = default;

    LoadedMesh* LoadMesh(const std::string& projectPath, const std::string& relativePath, float voxelSize);
    LoadedMesh* LoadMesh(Mesh&& mesh, const std::string& identifier);

    std::vector<LoadedMesh>& GetLoadedMeshes() {
        return loadedMeshes_;
    }

    bool RemoveMesh(const std::string& filePath);

    LoadedMesh* GetMeshByPath(const std::string& filePath);

    void Clear() {
        loadedMeshes_.clear();
    }

private:
    std::vector<LoadedMesh> loadedMeshes_;

    bool IsMeshLoaded(const std::string& filePath) const;
};
