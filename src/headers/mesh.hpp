#ifndef MESH_HPP
#define MESH_HPP

#include "vertex.hpp"
using namespace std;

// Each mesh is new object in GPU
struct Mesh
{
    unsigned int VAO = 0; // Vertex Array Object - memory w vertecies
    unsigned int VBO = 0; // Vertex Buffer Obj - instructions for reading vertex data
    size_t vertexCount = 0;
};

Mesh createMesh(const vector<Vertex>& vertices);

#endif