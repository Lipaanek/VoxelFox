#include "mesh_manager.hpp"
#include "../include/object_loader.hpp"
#include "../include/voxelizer.hpp"
#include "../include/color_loader.hpp"
#include <filesystem>
#include <cstdio>
#include <algorithm>
#include <utility>

LoadedMesh* MeshManager::LoadMesh(const std::string& projectPath, const std::string& relativePath, float voxelSize) {
    // Check if mesh is already loaded
    if (IsMeshLoaded(relativePath)) {
        printf("Mesh already loaded: %s\n", relativePath.c_str());
        return GetMeshByPath(relativePath);
    }

    // Construct full path to OBJ file
    std::filesystem::path fullPath = std::filesystem::path(projectPath) / "assets" / "objects" / relativePath;

    // Load OBJ file
    ObjectLoader loader;
    if (!loader.load(fullPath.string())) {
        printf("Failed to load mesh from: %s\n", fullPath.string().c_str());
        return nullptr;
    }

    // Try to load materials - look for MTL file in same directory
    std::vector<Material> materials;
    std::filesystem::path mtlPath = fullPath;
    mtlPath.replace_extension(".mtl");
    
    materials = loadColorFile(mtlPath.string());
    if (materials.empty()) {
        printf("Warning: No materials loaded for %s, using default\n", relativePath.c_str());
        materials.push_back(Material{Color{1.0f, 1.0f, 1.0f}, "default"});  // Default white material
    }

    // Voxelize the mesh
    std::vector<VoxelChunk> voxelChunks = voxelizeGPUCompute(
        loader.getVertices(),
        loader.getIndices(),
        loader.getTriangleMaterials(),
        voxelSize
    );

    if (voxelChunks.empty()) {
        printf("Voxelization produced no chunks for: %s\n", relativePath.c_str());
        return nullptr;
    }

    // Generate render mesh
    VoxelMesh voxelMesh = generateVoxelMesh(voxelChunks, materials);

    // Create LoadedMesh entry
    LoadedMesh loadedMesh;
    loadedMesh.filePath = relativePath;
    loadedMesh.voxelChunks = std::move(voxelChunks);
    loadedMesh.renderMesh = Mesh(voxelMesh.vertices, voxelMesh.indices);
    loadedMesh.isVoxelized = true;
    loadedMesh.modelMatrix = glm::mat4(1.0f);

    loadedMeshes_.push_back(std::move(loadedMesh));

    printf("Successfully loaded and voxelized mesh: %s\n", relativePath.c_str());
    return &loadedMeshes_.back();
}

LoadedMesh* MeshManager::LoadMesh(Mesh&& mesh, const std::string& identifier) {
    if (IsMeshLoaded(identifier)) {
        printf("Mesh already loaded: %s\n", identifier.c_str());
        return GetMeshByPath(identifier);
    }

    LoadedMesh loadedMesh(identifier, std::move(mesh));
    loadedMeshes_.push_back(std::move(loadedMesh));
    printf("Successfully loaded mesh from existing Mesh object: %s\n", identifier.c_str());
    return &loadedMeshes_.back();
}

bool MeshManager::RemoveMesh(const std::string& filePath) {
    auto it = std::find_if(loadedMeshes_.begin(), loadedMeshes_.end(),
        [&filePath](const LoadedMesh& mesh) { return mesh.filePath == filePath; });

    if (it != loadedMeshes_.end()) {
        loadedMeshes_.erase(it);
        printf("Removed mesh: %s\n", filePath.c_str());
        return true;
    }

    printf("Mesh not found: %s\n", filePath.c_str());
    return false;
}

LoadedMesh* MeshManager::GetMeshByPath(const std::string& filePath) {
    auto it = std::find_if(loadedMeshes_.begin(), loadedMeshes_.end(),
        [&filePath](const LoadedMesh& mesh) { return mesh.filePath == filePath; });

    if (it != loadedMeshes_.end()) {
        return &(*it);
    }

    return nullptr;
}

bool MeshManager::IsMeshLoaded(const std::string& filePath) const {
    return std::find_if(loadedMeshes_.begin(), loadedMeshes_.end(),
        [&filePath](const LoadedMesh& mesh) { return mesh.filePath == filePath; })
        != loadedMeshes_.end();
}
