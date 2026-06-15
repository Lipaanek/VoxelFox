#include "mesh_manager.hpp"
#include "../include/object_loader.hpp"
#include "../include/voxelizer.hpp"
#include "../include/color_loader.hpp"
#include "../include/voxel_model_io.hpp"
#include <filesystem>
#include <cstdio>
#include <algorithm>
#include <utility>
#include <glm/gtc/matrix_transform.hpp>

LoadedMesh* MeshManager::LoadMesh(const std::string& projectPath, const std::string& relativePath, float voxelSize) {
    namespace fs = std::filesystem;

    fs::path fullPath = fs::path(projectPath) / "assets" / "objects" / relativePath;
    fs::path voxfPath = fullPath;
    voxfPath.replace_extension(".voxf");

    // Load materials (needed in both cached and fresh paths)
    std::vector<Material> materials;
    fs::path mtlPath = fullPath;
    mtlPath.replace_extension(".mtl");
    materials = loadColorFile(mtlPath.string());
    if (materials.empty()) {
        printf("Warning: No materials loaded for %s, using default\n", relativePath.c_str());
        materials.push_back(Material{Color{1.0f, 1.0f, 1.0f}, "default"});
    }

    std::vector<VoxelChunk> voxelChunks;

    // Try loading from cached .voxf file (skip OBJ + GPU voxelization)
    if (fs::exists(voxfPath)) {
        voxelChunks = VoxelModelIO::LoadVoxelModel(voxfPath.string());
        if (!voxelChunks.empty()) {
            printf("Loaded cached voxel model: %s\n", voxfPath.string().c_str());
        } else {
            printf("Cached voxel file corrupt, re-voxelizing: %s\n", voxfPath.string().c_str());
        }
    }

    if (voxelChunks.empty()) {
        // Full pipeline: load OBJ and GPU voxelize
        ObjectLoader loader;
        if (!loader.load(fullPath.string())) {
            printf("Failed to load mesh from: %s\n", fullPath.string().c_str());
            return nullptr;
        }

        voxelChunks = voxelizeGPUCompute(
            loader.getVertices(),
            loader.getIndices(),
            loader.getTriangleMaterials(),
            voxelSize
        );

        if (voxelChunks.empty()) {
            printf("Voxelization produced no chunks for: %s\n", relativePath.c_str());
            return nullptr;
        }

        // Cache the voxel data to .voxf for future loads
        VoxelModelIO::SaveVoxelModel(voxfPath.string(), voxelChunks);
        printf("Cached voxel model to: %s\n", voxfPath.string().c_str());
    }

    // Generate render mesh from chunks + materials
    VoxelMesh voxelMesh = generateVoxelMesh(voxelChunks, materials);

    // Compute bounding box from generated mesh vertices
    glm::vec3 bmin(INFINITY), bmax(-INFINITY);
    for (const auto& v : voxelMesh.vertices) {
        bmin.x = std::min(bmin.x, v.position.x);
        bmin.y = std::min(bmin.y, v.position.y);
        bmin.z = std::min(bmin.z, v.position.z);
        bmax.x = std::max(bmax.x, v.position.x);
        bmax.y = std::max(bmax.y, v.position.y);
        bmax.z = std::max(bmax.z, v.position.z);
    }

    // Create new LoadedMesh entry (always, for multi-instance support)
    LoadedMesh loadedMesh;
    loadedMesh.filePath = relativePath;
    loadedMesh.voxelChunks = std::move(voxelChunks);
    loadedMesh.renderMesh = Mesh(voxelMesh.vertices, voxelMesh.indices);
    loadedMesh.isVoxelized = true;
    loadedMesh.bboxMin = bmin;
    loadedMesh.bboxMax = bmax;
    loadedMesh.translation = glm::vec3(0.0f);
    loadedMesh.modelMatrix = glm::translate(glm::mat4(1.0f), loadedMesh.translation);

    loadedMeshes_.push_back(std::move(loadedMesh));

    printf("Created instance of mesh: %s\n", relativePath.c_str());
    return &loadedMeshes_.back();
}

LoadedMesh* MeshManager::LoadMesh(Mesh&& mesh, const std::string& identifier) {
    LoadedMesh loadedMesh(identifier, std::move(mesh));
    loadedMeshes_.push_back(std::move(loadedMesh));
    printf("Loaded mesh from existing Mesh object: %s\n", identifier.c_str());
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


