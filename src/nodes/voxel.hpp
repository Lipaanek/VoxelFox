#pragma once

#include <glm/glm.hpp>
#include "../core/renderer/mesh/mesh.hpp"
#include "../core/renderer/mesh/mesh_data.hpp"

class Voxel {
private:
    glm::vec3 position { 0.0f };
    float size = 1.0f;
    glm::vec3 color { 1.0f };
    std::vector<glm::vec3> vertexColors;
    Mesh mesh;

public:
    Voxel() = default;
    Voxel(glm::vec3 position, float size, glm::vec3 color);
    ~Voxel() = default;

    [[nodiscard]] MeshData buildMeshData() const;
    void setup();
    void draw(const ShaderProgram&);

    void setPosition(glm::vec3 position);
    void setSize(float size);
    void setColor(glm::vec3 color);
    void setVertexColors(const std::vector<glm::vec3>& colors);
};