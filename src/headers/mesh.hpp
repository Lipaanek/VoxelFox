#ifndef MESH_HPP
#define MESH_HPP

#include <glad/glad.h>
#include <vector>
#include "vertex.hpp"

class Mesh {
public:
    Mesh() = default;
    Mesh(const std::vector<Vertex>& vertices);
    Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
    ~Mesh();

    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void draw() const;
    [[nodiscard]] GLsizei getIndexCount() const { return indexCount; }

private:
    GLuint VAO = 0; // Vertex Array Object
    GLuint VBO = 0; // Vertex Buffer Object
    GLuint EBO = 0; // Element Buffer Object
    GLsizei indexCount = 0;

    void setupMesh(const std::vector<Vertex>& vertices);
    void setupMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
};

#endif