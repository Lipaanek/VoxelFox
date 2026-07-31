#pragma once

#include "vertex.hpp"
#include <glad/glad.h>
#include <vector>

// Pure CPU mesh data
// decoupled from any OpenGL resources
struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
};
