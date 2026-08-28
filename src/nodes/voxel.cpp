#include "voxel.hpp"

#include <iterator>

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
    this->setName("Voxel");
}

Voxel::Voxel(const std::string &name) {
    this->setName(name);
}

MeshData Voxel::buildMeshData() const {
    MeshData data;

    data.vertices.reserve(kVertexCount);
    data.indices.assign(std::begin(kIndicesData), std::end(kIndicesData));

    const bool perVertex = vertexColors.size() == kVertexCount;

    for (int face = 0; face < kFaces; ++face) {
        for (int i = 0; i < kVerticesPerFace; ++i) {
            glm::vec3 vertexColor = perVertex ? vertexColors[data.vertices.size()] : color;
            data.vertices.emplace_back(
                getPosition() + size * kCubeCorners[face][i],
                kCubeNormals[face],
                kUvData[i],
                vertexColor);
        }
    }

    return data;
}

void Voxel::setup() {
    mesh.setData(buildMeshData());
    mesh.setup();
}

void Voxel::draw(const ShaderProgram& program) const {
    mesh.render(program);
}

void Voxel::setSize(float size) { this->size = size; }
void Voxel::setColor(glm::vec3 color) { this->color = color; }
void Voxel::setVertexColors(const std::vector<glm::vec3>& colors) { this->vertexColors = colors; }