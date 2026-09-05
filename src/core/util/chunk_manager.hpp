#pragma once

#include <glm/glm.hpp>
#include <map>
#include <tuple>
#include <vector>
#include <unordered_set>
#include <memory>

#include "../../nodes/mesh_instance_3d.hpp"
#include "aabb.hpp"

struct ChunkData {
    std::vector<MeshInstance3D*> objects;
    AABB aabb;
};

class ChunkManager {
private:
    float chunkSize;
    std::map<std::tuple<int, int, int>, ChunkData> chunks;
    std::unordered_set<const MeshInstance3D*> registeredNodes;

    std::tuple<int, int, int> getChunkCoords(const glm::vec3& position) const;

public:
    ChunkManager() : chunkSize(0.0f) {}
    explicit ChunkManager(float size);

    void reset(float size);
    void registerNode(MeshInstance3D* node);
    [[nodiscard]] size_t getChunkCount() const;
    [[nodiscard]] bool isRegistered(const MeshInstance3D* node) const;
    std::vector<ChunkData> getVisibleChunks(const Frustum& frustum) const;
};
