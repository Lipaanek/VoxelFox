#pragma once

#include <unordered_map>
#include "mesh.hpp"
#include "../shader_program.hpp"
#include "../../../nodes/transform3d.hpp"

using MeshID = unsigned int;

class MeshManager {
private:
    std::unordered_map<MeshID, Mesh> meshes;
    unsigned int nextUID = 0;

public:
    MeshID add(const MeshData &mesh);
    void update(MeshID uid, const MeshData& data);
    void remove(MeshID uid);
    void clear();

    size_t getSize() const;
    [[nodiscard]] Mesh& get(MeshID uid);
};