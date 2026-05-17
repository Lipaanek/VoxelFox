#pragma once

#include <string>
#include <vector>
#include "../include/loaded_mesh.hpp"

class MeshManager {
public:
    MeshManager() = default;
    ~MeshManager() = default;

    /**
     * Load a mesh from file, voxelize it, and add to loaded meshes
     * @param projectPath Full path to the project directory
     * @param relativePath Relative path to OBJ file from projectPath/assets/objects/
     * @param voxelSize Size of voxels for voxelization
     * @return Pointer to the loaded LoadedMesh, or nullptr if failed
     */
    LoadedMesh* LoadMesh(const std::string& projectPath, const std::string& relativePath, float voxelSize);

    /**
     * Get all currently loaded meshes
     * @return Vector of loaded meshes
     */
    std::vector<LoadedMesh>& GetLoadedMeshes() {
        return loadedMeshes_;
    }

    /**
     * Remove a mesh from memory by file path
     * @param filePath Relative file path to remove
     * @return True if mesh was found and removed, false otherwise
     */
    bool RemoveMesh(const std::string& filePath);

    /**
     * Get a specific mesh by file path
     * @param filePath Relative file path to find
     * @return Pointer to LoadedMesh if found, nullptr otherwise
     */
    LoadedMesh* GetMeshByPath(const std::string& filePath);

    /**
     * Clear all loaded meshes
     */
    void Clear() {
        loadedMeshes_.clear();
    }

private:
    std::vector<LoadedMesh> loadedMeshes_;

    /**
     * Check if a mesh with the given path is already loaded
     * @param filePath Path to check
     * @return True if mesh already loaded
     */
    bool IsMeshLoaded(const std::string& filePath) const;
};
