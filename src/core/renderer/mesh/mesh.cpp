#include "mesh.hpp"
#include "../../util/util.hpp"
#include "../shader_program.hpp"

void Mesh::setup() const {
    if (this->vertices.empty() || this->indices.empty()) {
        Util::Log::error("Mesh::setup() called without vertices or indices");
        return;
    }

    this->vao.bind();
    this->vbo.upload(this->vertices.data(), this->vertices.size() * sizeof(Vertex));
    this->ebo.upload(this->indices.data(), this->indices.size() * sizeof(GLuint));

    // Upload the vertex attributes based on the layout
    this->layout3D.upload();
}

void Mesh::setData(const MeshData& data) {
    this->vertices = data.vertices;
    this->indices = data.indices;
}

void Mesh::setVertices(std::vector<Vertex> vertices) {
    this->vertices = vertices;
}

void Mesh::setIndices(std::vector<GLuint> indices) {
    this->indices = indices;
}

void Mesh::render(const ShaderProgram& program) const {
    program.use();

    vao.bind();

    // draw mesh
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(this->indices.size()), GL_UNSIGNED_INT, nullptr);
}
