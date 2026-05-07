#include "../headers/mesh.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace std;

Mesh createMesh(const vector<Vertex>& vertices)
{
    Mesh mesh;
    mesh.vertexCount = (int)vertices.size();

    glGenVertexArrays(1, &mesh.VAO); // Make the vert array
    glGenBuffers(1, &mesh.VBO); // Make the buffer

    glBindVertexArray(mesh.VAO); // Add vert array

    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 vertices.size() * sizeof(Vertex),
                 vertices.data(), // set the buffer data onto the GPU memory
                 GL_STATIC_DRAW);

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*) offsetof(Vertex, position)
    );
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex), 
        (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(
        2, 
        2, 
        GL_FLOAT, 
        GL_FALSE, 
        sizeof(Vertex), 
        (void*)offsetof(Vertex, uv));
    glEnableVertexAttribArray(2);

    return mesh;
}