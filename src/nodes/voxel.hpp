#pragma once

#include "node3d.hpp"
#include <glm/glm.hpp>
#include "../core/renderer/mesh/mesh.hpp"
#include "../core/renderer/mesh/mesh_data.hpp"

class Voxel : public Node3D {
private:
    float size = 1.0f;
    glm::vec3 color { 1.0f };
    std::vector<glm::vec3> vertexColors;
    Mesh mesh;

public:
    Voxel();
    explicit Voxel(const std::string& name);
    ~Voxel() override = default;

    [[nodiscard]] MeshData buildMeshData() const;
    void setup();
    void draw(const ShaderProgram&) const;

    void setSize(float size);
    void setColor(glm::vec3 color);
    void setVertexColors(const std::vector<glm::vec3>& colors);
};