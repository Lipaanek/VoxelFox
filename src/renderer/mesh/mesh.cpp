#include "mesh.hpp"

Mesh::~Mesh() {
    if (vao) {
        glDeleteVertexArrays(1, &vao);
    }
    if (vao) {
        glDeleteBuffers(1, &vbo);
    }
    if (vao) {
        glDeleteBuffers(1, &ebo);
    }
}

void Mesh::setVerticies(std::vector<Vertex> verticies) {
    this->verticies = verticies;
}

void Mesh::setIndicies(std::vector<Vertex> indicies) {
    this->verticies = verticies;
}

void Mesh::draw(Shader &shader) {

}