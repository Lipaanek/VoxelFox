#ifndef VOXELIZER_HPP
#define VOXELIZER_HPP

#include "vertex.hpp"
#include "voxel.hpp"
#include <string>
#include <vector>

using namespace std;

vector<Voxel> voxelize(vector<Vertex> vert, float voxelSize = 1.0f);
vector<VoxelChunk> voxelizeGPU(vector<Vertex> vertices, float voxelSize = 0.1f);

void saveChunks(const vector<VoxelChunk>& chunks, const string& path);
vector<VoxelChunk> loadChunks(const string& path);

#endif