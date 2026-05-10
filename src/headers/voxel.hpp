#ifndef VOXEL_HPP
#define VOXEL_HPP

#include <glm/glm.hpp>
#include <vector>

struct Voxel {
    glm::ivec4 data{};
    bool solid = false;
};

struct VoxelChunk {
    glm::ivec3 chunkPos{};
    glm::ivec3 chunkSize{};
    float voxelSize = 1.0f;
    glm::vec3 gridOrigin{};
    std::vector<Voxel> voxels;
};

#endif