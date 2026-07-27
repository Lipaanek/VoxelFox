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

void Mesh::setup() {
    // Generate all the buffers
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Add vertex data
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &(this->vertices)[0], GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), 
                 &(this->indices)[0], GL_STATIC_DRAW);

    // Vertex positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glBindVertexArray(0);
}

void Mesh::setVertices(std::vector<Vertex> vertices) {
    this->vertices = vertices;
}

void Mesh::setIndices(std::vector<GLuint> indices) {
    this->vertices = vertices;
}

void Mesh::draw(Shader &shader) {

}