#ifndef VOXELIZER_HPP
#define VOXELIZER_HPP

#include "vertex.hpp"
#include "voxel.hpp"
#include "color_loader.hpp"
#include <vector>
#include <glad/glad.h>

std::vector<VoxelChunk> voxelizeGPUCompute(std::vector<Vertex> vertices, std::vector<uint32_t> indices, const std::vector<int>& triMaterials, float voxelSize);

struct VoxelMesh {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

VoxelMesh generateVoxelMesh(const std::vector<VoxelChunk>& chunks, const std::vector<Material>& materials);

#endif