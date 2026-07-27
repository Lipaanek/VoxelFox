#pragma once

#include "vertex_buffer.cpp"
#include "vertex_array.cpp"
#include <vector>
#include <glad/glad.h>

class Mesh {
private:
    GLuint voa = 0; // Vertex Array Object (data)
    GLuint vbo = 0; // Vertex Buffer Object (describes how to organize data)
    GLuint ebo = 0; // Element Buffer Object
    GLsizei indexCount = 0;

    void setupMesh();

public:
    std::vector<Vertex> verticies;
    std::vector<GLuint> indicies;

    Mesh() {}
    ~Mesh();

    void setVerticies(std::vector<Vertex> verticies);
    void setIndicies(std::vector<GLuint> indicies);

    void draw(Shader &shader);
};