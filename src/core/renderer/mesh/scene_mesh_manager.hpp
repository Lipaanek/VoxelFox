#pragma once

#include <unordered_map>
#include "mesh.hpp"
#include <glm/glm.hpp>

struct MeshEntry {
    Mesh mesh;
    glm::mat4 model { 1.0f };
};

class SceneMeshManager {
private:
    std::unordered_map<unsigned int, MeshEntry> meshes;
    unsigned int nextUID = 0;

public:
    unsigned int add(const MeshData& mesh, const glm::mat4& model = glm::mat4(1.0f));
    void setTransform(unsigned int uid, const glm::mat4 model);
    [[nodiscard]] glm::mat4 getTransform(unsigned int uid) const;
    void update(unsigned int uid, const MeshData& mesh);
    void remove(unsigned int uid);
    void clear();
    void render(ShaderProgram& program);
    Mesh& get(unsigned int uid);
};