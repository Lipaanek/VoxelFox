#pragma once

#include "mesh/mesh_data.hpp"
#include <glm/glm.hpp>
#include <vector>

struct GPUVoxel {
    glm::vec3 position;
    float size;
    glm::vec3 color;
    float padding; // matches GLSL std430 alignment (32 bytes per element)
};

class Voxelizer {
public:
    std::vector<GPUVoxel> voxelize(const MeshData& mesh, float gridSize);
};
