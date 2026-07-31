#include "mesh.hpp"
#include "../../util/util.hpp"
#include "../shader_program.hpp"

void Mesh::setup() {
    if (this->vertices.empty() || this->indices.empty()) {
        Util::Log::error("Mesh::setup() called without vertices or indices");
        return;
    }

    this->vao.bind();
    this->vbo.upload(this->vertices.data(), this->vertices.size() * sizeof(Vertex));
    this->ebo.upload(this->indices.data(), this->indices.size() * sizeof(GLuint));

    for (const VertexAttribute& a : this->layout) {
        glEnableVertexAttribArray(a.location);
        glVertexAttribPointer(a.location, a.size, a.type, GL_FALSE, sizeof(Vertex), (void*)a.offset);
    }
}

void Mesh::setVertices(std::vector<Vertex> vertices) {
    this->vertices = vertices;
}

void Mesh::setIndices(std::vector<GLuint> indices) {
    this->indices = indices;
}

void Mesh::draw(const ShaderProgram& program) {
    program.use();

    vao.bind();

    // draw mesh
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(this->indices.size()), GL_UNSIGNED_INT, 0);
}