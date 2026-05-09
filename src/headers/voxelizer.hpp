#ifndef VOXELIZER_HPP
#define VOXELIZER_HPP

#include "vertex.hpp"
#include "voxel.hpp"
#include <vector>
#include <glad/glad.h>

std::vector<VoxelChunk> voxelizeGPU(std::vector<Vertex> vertices, std::vector<uint32_t> indicies , float voxelSize = 0.1f);
std::vector<VoxelChunk> voxelizeGPUCompute(std::vector<Vertex> vertices, std::vector<uint32_t> indices, float voxelSize);

struct VoxelMesh {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

VoxelMesh generateVoxelMesh(const std::vector<VoxelChunk>& chunks);

#endif