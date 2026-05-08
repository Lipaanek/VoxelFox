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

void addCube(const glm::vec3& p, float s, vector<Vertex>& out) {
    glm::vec3 h = glm::vec3(s * 0.5f);

    glm::vec3 v[8] = {
        p + glm::vec3(-h.x, -h.y, -h.z),
        p + glm::vec3( h.x, -h.y, -h.z),
        p + glm::vec3( h.x,  h.y, -h.z),
        p + glm::vec3(-h.x,  h.y, -h.z),
        p + glm::vec3(-h.x, -h.y,  h.z),
        p + glm::vec3( h.x, -h.y,  h.z),
        p + glm::vec3( h.x,  h.y,  h.z),
        p + glm::vec3(-h.x,  h.y,  h.z)
    };

    int faces[36] = {
        0,1,2, 2,3,0,
        1,5,6, 6,2,1,
        5,4,7, 7,6,5,
        4,0,3, 3,7,4,
        3,2,6, 6,7,3,
        4,5,1, 1,0,4
    };

    glm::vec3 normals[6] = {
        { 0,0,-1 }, { 1,0,0 }, { 0,0,1 },
        { -1,0,0 }, { 0,1,0 }, { 0,-1,0 }
    };

    for (int i = 0; i < 36; i += 3) {
        int f0 = faces[i];
        int f1 = faces[i+1];
        int f2 = faces[i+2];

        glm::vec3 normal = normals[i / 6];

        out.push_back({ v[f0], normal, {0,0} });
        out.push_back({ v[f1], normal, {0,0} });
        out.push_back({ v[f2], normal, {0,0} });
    }
}

vector<Vertex> turnIntoVertices(const vector<Voxel>& voxels) {
    vector<Vertex> vertices;

    float voxelSize = 0.1f;

    for (const Voxel& v : voxels) {
        if (!v.solid) continue;

        glm::vec3 worldPos = glm::vec3(v.position) * voxelSize;

        addCube(worldPos, voxelSize, vertices);
    }

    return vertices;
}

Mesh createMesh(const vector<Voxel>& voxels) {
    return createMesh(turnIntoVertices(voxels));
}