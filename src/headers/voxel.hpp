#ifndef VOXEL_HPP
#define VOXEL_HPP

#include <glm/glm.hpp>
#include <functional>

using namespace std;

struct Voxel {
    glm::ivec3 position;
    bool solid;
};

struct VoxelHash {
    size_t operator()(const glm::ivec3& v) const {
        uint64_t x = static_cast<uint32_t>(v.x);
        uint64_t y = static_cast<uint32_t>(v.y);
        uint64_t z = static_cast<uint32_t>(v.z);
        uint64_t h = (x * 73856093) ^ (y * 19349663) ^ (z * 83492791);
        return static_cast<size_t>(h);
    }
};

#endif