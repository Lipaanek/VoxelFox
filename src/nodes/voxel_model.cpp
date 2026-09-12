#include "voxel_model.hpp"

#include <array>
#include <cmath>

#include "core/model_loading/voxelizer.hpp"
#include "core/scene/scene.hpp"

namespace {

constexpr int kVerticesPerFace = 4;
constexpr int kFaces = 6;
constexpr int kVertexCount = kFaces * kVerticesPerFace;

constexpr glm::vec3 kCubeNormals[kFaces] = {
    { 0.0f,  0.0f, -1.0f },
    { 0.0f,  0.0f,  1.0f },
    {-1.0f,  0.0f,  0.0f },
    { 1.0f,  0.0f,  0.0f },
    { 0.0f, -1.0f,  0.0f },
    { 0.0f,  1.0f,  0.0f }
};

constexpr glm::vec3 kCubeCorners[kFaces][kVerticesPerFace] = {
    { {-0.5f,-0.5f,-0.5f}, { 0.5f,-0.5f,-0.5f}, { 0.5f, 0.5f,-0.5f}, {-0.5f, 0.5f,-0.5f} },
    { {-0.5f,-0.5f, 0.5f}, { 0.5f,-0.5f, 0.5f}, { 0.5f, 0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f} },
    { {-0.5f,-0.5f,-0.5f}, {-0.5f,-0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f}, {-0.5f, 0.5f,-0.5f} },
    { { 0.5f,-0.5f, 0.5f}, { 0.5f,-0.5f,-0.5f}, { 0.5f, 0.5f,-0.5f}, { 0.5f, 0.5f, 0.5f} },
    { {-0.5f,-0.5f, 0.5f}, { 0.5f,-0.5f, 0.5f}, { 0.5f,-0.5f,-0.5f}, {-0.5f,-0.5f,-0.5f} },
    { {-0.5f, 0.5f,-0.5f}, { 0.5f, 0.5f,-0.5f}, { 0.5f, 0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f} }
};

constexpr glm::vec2 kUvData[kVerticesPerFace] = {
    { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f }
};

}

VoxelModel::VoxelModel() {
    this->setName("VoxelModel");
}

VoxelModel::VoxelModel(const std::string& name, float gridSize)
    : MeshInstance3D(name), gridSize(gridSize) {
}

VoxelModel::~VoxelModel() = default;

glm::ivec3 VoxelModel::worldToGrid(const glm::vec3 position) const {
    return glm::ivec3(glm::floor(position / gridSize));
}

glm::vec3 VoxelModel::gridToWorld(const glm::ivec3 position) const {
    return glm::vec3(position) * gridSize;
}

glm::ivec3 VoxelModel::snapToGrid(const glm::vec3 worldPos) const {
    return worldToGrid(worldPos);
}

VoxelHash VoxelModel::computeHash(const glm::ivec3 pos) const {
    uint32_t ux = static_cast<uint32_t>(pos.x) ^ 0x80000000u;
    uint32_t uy = static_cast<uint32_t>(pos.y) ^ 0x80000000u;
    uint32_t uz = static_cast<uint32_t>(pos.z) ^ 0x80000000u;

    uint64_t h = 1469598103934665603ULL;
    h ^= ux; h *= 1099511628211ULL;
    h ^= uy; h *= 1099511628211ULL;
    h ^= uz; h *= 1099511628211ULL;
    return h;
}

void VoxelModel::voxelize(const MeshData &mesh) {
    Voxelizer voxelizer;
    std::vector<GPUVoxel> gpuVoxels = voxelizer.voxelize(mesh, this->gridSize);

    glm::vec3 minCorner = mesh.vertices[0].position;
    for (const auto& vertex : mesh.vertices) {
        minCorner = glm::min(minCorner, vertex.position);
    }

    for (const auto& voxel : gpuVoxels) {
        const glm::ivec3 gridPos = worldToGrid(voxel.position);

        const VoxelHash hash = computeHash(gridPos);

        if (voxelSet.contains(hash))
            continue;

        voxelSet.insert(hash);
        voxelPositions.push_back(gridPos);
        voxelColors.push_back(voxel.color);
    }

    this->status = ModelStatus::Unbaked;

    if (this->getScene() == nullptr) return;

    this->bake();
}

void VoxelModel::addVoxel(glm::vec3 worldPos, glm::vec3 color) {
    const glm::ivec3 gridPos = snapToGrid(worldPos);
    const VoxelHash hash = computeHash(gridPos);

    if (voxelSet.contains(hash)) return;

    voxelSet.insert(hash);
    voxelPositions.emplace_back(gridPos);
    voxelColors.emplace_back(color);

    this->status = ModelStatus::Unbaked;
}

MeshData VoxelModel::buildMeshData() const {
    MeshData data;

    data.vertices.reserve(kVertexCount * static_cast<size_t>(voxelPositions.size()));
    data.indices.reserve(6 * static_cast<size_t>(voxelPositions.size()));

    std::array<bool, kFaces> visibleFaces{};

    size_t vertexOffset = 0;

    for (size_t i = 0; i < voxelPositions.size(); ++i) {
        glm::ivec3 pos = voxelPositions[i];
        glm::vec3 worldCenter = glm::vec3(pos) * gridSize;
        glm::vec3 color = voxelColors[i];

        const auto checkNeighbor = [&](const int dx, const int dy, const int dz) -> bool {
            return voxelSet.contains(computeHash(glm::ivec3(pos.x + dx, pos.y + dy, pos.z + dz)));
        };

        visibleFaces[0] = !checkNeighbor( 0,  0, -1);
        visibleFaces[1] = !checkNeighbor( 0,  0,  1);
        visibleFaces[2] = !checkNeighbor(-1,  0,  0);
        visibleFaces[3] = !checkNeighbor( 1,  0,  0);
        visibleFaces[4] = !checkNeighbor( 0, -1,  0);
        visibleFaces[5] = !checkNeighbor( 0,  1,  0);

        for (int face = 0; face < kFaces; ++face) {
            if (!visibleFaces[face]) continue;

            for (int v = 0; v < kVerticesPerFace; ++v) {
                glm::vec3 worldPos = worldCenter + kCubeCorners[face][v] * gridSize;
                data.vertices.emplace_back(
                    worldPos,
                    kCubeNormals[face],
                    kUvData[v],
                    color
                );
            }

            data.indices.push_back(static_cast<GLuint>(vertexOffset + 0));
            data.indices.push_back(static_cast<GLuint>(vertexOffset + 1));
            data.indices.push_back(static_cast<GLuint>(vertexOffset + 2));
            data.indices.push_back(static_cast<GLuint>(vertexOffset + 2));
            data.indices.push_back(static_cast<GLuint>(vertexOffset + 3));
            data.indices.push_back(static_cast<GLuint>(vertexOffset + 0));

            vertexOffset += kVerticesPerFace;
        }
    }

    return data;
}

void VoxelModel::bake() {
    const MeshData meshData = buildMeshData();

    if (auto* scene = getScene()) {
        auto meshID = static_cast<MeshID>(-1);
        if (getMesh() != static_cast<MeshID>(-1)) {
            scene->getMeshManager().update(getMesh(), meshData);
            meshID = getMesh();
        } else {
            auto [id] = scene->getMeshManager().add(meshData);
            meshID = id;
        }
        setMesh(meshID);
    }

    this->status = ModelStatus::Baked;
}

void VoxelModel::onTreeEnter(Scene* newScene) {
    (void)newScene;
    this->bake();
}
