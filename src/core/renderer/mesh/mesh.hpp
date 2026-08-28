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
            { .location = 0, .size = 3, .type = GL_FLOAT, .offset = offsetof(Vertex, position)},
            { .location = 1, .size = 3, .type = GL_FLOAT, .offset = offsetof(Vertex, normal)},
            { .location = 2, .size = 2, .type = GL_FLOAT, .offset = offsetof(Vertex, texCoord)},
            { .location = 3, .size = 3, .type = GL_FLOAT, .offset = offsetof(Vertex, color)},
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
