#pragma once

#include <unordered_map>
#include <functional>
#include "mesh.hpp"
#include "../shader_program.hpp"

using MeshID = unsigned int;

struct MeshResult {
    MeshID id;
};

class MeshManager {
private:
    std::unordered_map<MeshID, Mesh> meshes;
    std::unordered_map<float, MeshID> sizeCache;
    MeshID nextUID = 0;

public:
    MeshResult add(const MeshData &mesh);
    MeshResult getOrCreate(float size, const std::function<MeshData()>& factory);

    void update(MeshID uid, const MeshData& data);
    void remove(MeshID uid);
    void clear();

    size_t getSize() const;
    [[nodiscard]] Mesh& get(MeshID uid);
};