#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "mesh.hpp"
#include "voxel.hpp"

struct LoadedMesh {
    std::string filePath;                        // Relative path to OBJ file within project
    std::vector<VoxelChunk> voxelChunks;        // Voxelized chunks
    Mesh renderMesh;                             // GPU mesh for rendering
    bool isVoxelized = false;                    // Whether voxelization is complete
    glm::mat4 modelMatrix = glm::mat4(1.0f);   // Model transformation matrix (identity by default)

    LoadedMesh() = default;
    
    // Constructor with file path
    LoadedMesh(const std::string& path)
        : filePath(path), isVoxelized(false), modelMatrix(glm::mat4(1.0f)) {}
};
