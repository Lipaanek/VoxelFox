#pragma once

#include "vertex.hpp"
#include "../shader.hpp"
#include <vector>
#include <glad/glad.h>

class Mesh {
private:
    GLuint vao = 0; // Vertex Array Object (data)
    GLuint vbo = 0; // Vertex Buffer Object (describes how to organize data)
    GLuint ebo = 0; // Element Buffer Object
    GLsizei indexCount = 0;

public:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    Mesh() {}
    ~Mesh();

    void setVertices(std::vector<Vertex> vertices);
    void setIndices(std::vector<GLuint> indices);

    void draw(GLuint programID);
    void setup();
};