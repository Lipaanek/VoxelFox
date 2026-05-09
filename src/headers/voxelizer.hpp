#ifndef VOXELIZER_HPP
#define VOXELIZER_HPP

#include "vertex.hpp"
#include "voxel.hpp"
#include <vector>

std::vector<VoxelChunk> voxelizeGPU(std::vector<Vertex> vertices, float voxelSize = 0.1f);

#endif