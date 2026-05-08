#ifndef MESH_HPP
#define MESH_HPP

#include "vertex.hpp"
#include "voxel.hpp"
using namespace std;

struct Mesh
{
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    size_t vertexCount = 0;
};

Mesh createMesh(const vector<Vertex>& vertices);
Mesh createMesh(const vector<Voxel>& voxels);
Mesh createMesh(const vector<VoxelChunk>& chunks);

#endif