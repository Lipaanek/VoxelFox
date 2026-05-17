#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "mesh.hpp"
#include "voxel.hpp"

struct LoadedMesh {
    std::string filePath;
    std::vector<VoxelChunk> voxelChunks;
    Mesh renderMesh;
    bool isVoxelized = false;
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    LoadedMesh() = default;
    
    // Constructor with file path
    LoadedMesh(const std::string& path)
        : filePath(path), isVoxelized(false), modelMatrix(glm::mat4(1.0f)) {}

    // Constructor from a prebuilt render mesh
    LoadedMesh(const std::string& identifier, Mesh&& mesh)
        : filePath(identifier), renderMesh(std::move(mesh)), isVoxelized(false), modelMatrix(glm::mat4(1.0f)) {}
};
