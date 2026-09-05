#include "chunk_manager.hpp"
#include "frustum_culling.hpp"

ChunkManager::ChunkManager(const float size) : chunkSize(size) {}

void ChunkManager::reset(const float size) {
    chunkSize = size;
    chunks.clear();
    registeredNodes.clear();
}

std::tuple<int, int, int> ChunkManager::getChunkCoords(const glm::vec3& position) const {
    float size = chunkSize;
    auto coord = [size](const float val) {
        return static_cast<int>(glm::floor(val / size));
    };
    return {coord(position.x), coord(position.y), coord(position.z)};
}

void ChunkManager::registerNode(MeshInstance3D* node) {
    if (registeredNodes.contains(node)) return;

    const glm::vec3 pos = node->getPosition();
    const auto [cx, cy, cz] = getChunkCoords(pos);
    const auto key = std::make_tuple(cx, cy, cz);

    BoundingSphere sphere = node->getGlobalBoundingSphere();
    glm::vec3 childMin = sphere.center - glm::vec3(sphere.radius);
    glm::vec3 childMax = sphere.center + glm::vec3(sphere.radius);

    auto it = chunks.find(key);
    if (it == chunks.end()) {
        ChunkData chunk;
        chunk.objects.push_back(node);
        chunk.aabb.min = childMin;
        chunk.aabb.max = childMax;
        chunks[key] = chunk;
    } else {
        it->second.objects.push_back(node);
        it->second.aabb.min = glm::min(it->second.aabb.min, childMin);
        it->second.aabb.max = glm::max(it->second.aabb.max, childMax);
    }

    registeredNodes.insert(node);
}

size_t ChunkManager::getChunkCount() const {
    return chunks.size();
}

bool ChunkManager::isRegistered(const MeshInstance3D* node) const {
    return registeredNodes.contains(node);
}

std::vector<ChunkData> ChunkManager::getVisibleChunks(const Frustum& frustum) const {
    std::vector<ChunkData> visible;
    visible.reserve(chunks.size());

    for (const auto& [coords, chunkData] : chunks) {
        if (chunkData.aabb.intersects(frustum)) {
            visible.push_back(chunkData);
        }
    }

    return visible;
}
