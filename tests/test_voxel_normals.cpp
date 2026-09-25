#include "catch_amalgamated.hpp"

#include "nodes/voxel.hpp"
#include "nodes/voxel_model.hpp"

namespace {

void requireOutwardTriangleNormals(const MeshData& mesh) {
    REQUIRE(mesh.indices.size() % 3 == 0);
    for (size_t index = 0; index < mesh.indices.size(); index += 3) {
        const Vertex& a = mesh.vertices[mesh.indices[index]];
        const Vertex& b = mesh.vertices[mesh.indices[index + 1]];
        const Vertex& c = mesh.vertices[mesh.indices[index + 2]];
        const glm::vec3 geometricNormal = glm::cross(b.position - a.position, c.position - a.position);
        REQUIRE(glm::dot(geometricNormal, a.normal) > 0.0f);
    }
}

}

TEST_CASE("Voxel faces wind consistently with outward normals") {
    Voxel voxel;
    requireOutwardTriangleNormals(voxel.buildMeshData());
}

TEST_CASE("Voxel model faces wind consistently with outward normals and preserve color") {
    VoxelModel model("test", 1.0f);
    const glm::vec3 color { 0.4f, 0.2f, 0.8f };
    model.addVoxel({ 0.5f, 0.5f, 0.5f }, color);

    const MeshData mesh = model.buildMeshData();
    REQUIRE(mesh.vertices.size() == 24);
    requireOutwardTriangleNormals(mesh);

    for (const Vertex& vertex : mesh.vertices)
        REQUIRE(vertex.color == color);
}
