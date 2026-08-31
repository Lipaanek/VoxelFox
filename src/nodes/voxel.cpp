#include "voxel.hpp"

#include <iterator>

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

constexpr GLuint kIndicesData[] = {
    0, 1, 2,  2, 3, 0,
    4, 5, 6,  6, 7, 4,
    8, 9, 10, 10, 11, 8,
    12, 13, 14, 14, 15, 12,
    16, 17, 18, 18, 19, 16,
    20, 21, 22, 22, 23, 20,
};

const glm::vec2 kUvData[kVerticesPerFace] = {
    { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f }
};

}

Voxel::Voxel() {
    this->name = "Voxel";
}

Voxel::Voxel(const std::string &name) : MeshInstance3D(name) {
}

void Voxel::onTreeEnter(Scene *newScene) {
    if (this->getMesh() != static_cast<MeshID>(-1)) return;

    const auto [id, aabb] = newScene->getMeshManager().getOrCreate(size, [this]() { return buildMeshData(); });
    setMesh(id, aabb);
}

void Voxel::onTreeExit(Scene *currentScene) {
}

Voxel::~Voxel() {
}

MeshData Voxel::buildMeshData() const {
    MeshData data;

    data.vertices.reserve(kVertexCount);
    data.indices.assign(std::begin(kIndicesData), std::end(kIndicesData));

    for (int face = 0; face < kFaces; ++face) {
        for (int i = 0; i < kVerticesPerFace; ++i) {
            data.vertices.emplace_back(
                size * kCubeCorners[face][i],
                kCubeNormals[face],
                kUvData[i],
                glm::vec3(1.0f)
            );
        }
    }

    return data;
}

void Voxel::setSize(float size) {
    this->size = size;
    if (auto* scene = this->getScene()) {
        const auto [id, aabb] = scene->getMeshManager().getOrCreate(size, [this]() { return buildMeshData(); });
        setMesh(id, aabb);
    }
}