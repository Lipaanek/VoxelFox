#pragma once

#include "vertex.hpp"
#include "mesh_data.hpp"
#include "../buffer.hpp"
#include "../vertex_array.hpp"
#include <vector>
#include "../shader_program.hpp"
#include "core/renderer/layout.hpp"

class Mesh {
private:
    VertexArray vao; // Vertex Array Object (data)
    Buffer vbo { GL_ARRAY_BUFFER }; // Vertex Buffer Object (describes how to organize data)
    Buffer ebo { GL_ELEMENT_ARRAY_BUFFER }; // Element Buffer Object

    // Layout of how to upload the layout to the shader
    Layout layout3D = {
        {
            { 0, 3, GL_FLOAT, offsetof(Vertex, position) },
            { 1, 3, GL_FLOAT, offsetof(Vertex, normal) },
            { 2, 2, GL_FLOAT, offsetof(Vertex, texCoord) },
            { 3, 3, GL_FLOAT, offsetof(Vertex, color) },
        }, sizeof(Vertex)
    };

public:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    Mesh() = default;
    void setData(const MeshData& data);
    void setVertices(std::vector<Vertex> vertices);
    void setIndices(std::vector<GLuint> indices);

    void render(const ShaderProgram& program) const;
    void setup() const;
};
