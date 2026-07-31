#include "catch_amalgamated.hpp"
#include "core/model_loading/obj_loader.hpp"

#include <fstream>
#include <cmath>

namespace {
    const std::string OBJ_PATH = "obj_loader_test.obj";
    const std::string MTL_PATH = "obj_loader_test.mtl";

    void writeFile(const std::string& path, const std::string& contents) {
        std::ofstream file(path);
        file << contents;
    }

    bool approxEqual(float a, float b) {
        return std::abs(a - b) < 1e-5f;
    }

    bool approxEqual(const glm::vec3& a, const glm::vec3& b) {
        return approxEqual(a.x, b.x) && approxEqual(a.y, b.y) && approxEqual(a.z, b.z);
    }
}

TEST_CASE("ObjLoader loads a triangle with MTL colors") {
    writeFile(MTL_PATH, "newmtl red\nKd 1.0 0.0 0.0\n");
    writeFile(OBJ_PATH,
              "mtllib obj_loader_test.mtl\n"
              "usemtl red\n"
              "v 0.0 0.0 0.0\n"
              "v 1.0 0.0 0.0\n"
              "v 0.0 1.0 0.0\n"
              "f 1 2 3\n");

    ObjLoader loader;
    MeshData data = loader.Load(OBJ_PATH);

    REQUIRE(data.vertices.size() == 3);
    REQUIRE(data.indices.size() == 3);
    REQUIRE(data.indices[0] == 0);
    REQUIRE(data.indices[1] == 1);
    REQUIRE(data.indices[2] == 2);

    for (const Vertex& v : data.vertices) {
        REQUIRE(approxEqual(v.color, glm::vec3(1.0f, 0.0f, 0.0f)));
    }
    REQUIRE(approxEqual(data.vertices[0].position, glm::vec3(0.0f, 0.0f, 0.0f)));
    REQUIRE(approxEqual(data.vertices[1].position, glm::vec3(1.0f, 0.0f, 0.0f)));

    // No normals in the OBJ, so a flat face normal is computed.
    REQUIRE(approxEqual(data.vertices[0].normal, glm::vec3(0.0f, 0.0f, 1.0f)));

    std::remove(OBJ_PATH.c_str());
    std::remove(MTL_PATH.c_str());
}

TEST_CASE("ObjLoader fan-triangulates quads") {
    writeFile(OBJ_PATH,
              "v 0.0 0.0 0.0\n"
              "v 1.0 0.0 0.0\n"
              "v 1.0 1.0 0.0\n"
              "v 0.0 1.0 0.0\n"
              "f 1 2 3 4\n");

    ObjLoader loader;
    MeshData data = loader.Load(OBJ_PATH);

    REQUIRE(data.vertices.size() == 4);
    REQUIRE(data.indices.size() == 6);

    std::remove(OBJ_PATH.c_str());
}

TEST_CASE("ObjLoader de-duplicates shared vertices") {
    writeFile(OBJ_PATH,
              "v 0.0 0.0 0.0\n"
              "v 1.0 0.0 0.0\n"
              "v 0.0 1.0 0.0\n"
              "v 1.0 1.0 0.0\n"
              "f 1 2 3\n"
              "f 2 4 3\n");

    ObjLoader loader;
    MeshData data = loader.Load(OBJ_PATH);

    REQUIRE(data.vertices.size() == 4);
    REQUIRE(data.indices.size() == 6);

    std::remove(OBJ_PATH.c_str());
}

TEST_CASE("ObjLoader uses white when no material is set") {
    writeFile(OBJ_PATH,
              "v 0.0 0.0 0.0\n"
              "v 1.0 0.0 0.0\n"
              "v 0.0 1.0 0.0\n"
              "f 1 2 3\n");

    ObjLoader loader;
    MeshData data = loader.Load(OBJ_PATH);

    REQUIRE(data.vertices.size() == 3);
    for (const Vertex& v : data.vertices) {
        REQUIRE(approxEqual(v.color, glm::vec3(1.0f)));
    }

    std::remove(OBJ_PATH.c_str());
}

TEST_CASE("ObjLoader supports negative (relative) indices") {
    writeFile(OBJ_PATH,
              "v 0.0 0.0 0.0\n"
              "v 1.0 0.0 0.0\n"
              "v 0.0 1.0 0.0\n"
              "f -3 -2 -1\n");

    ObjLoader loader;
    MeshData data = loader.Load(OBJ_PATH);

    REQUIRE(data.vertices.size() == 3);
    REQUIRE(data.indices.size() == 3);
    REQUIRE(data.indices[0] == 0);
    REQUIRE(data.indices[1] == 1);
    REQUIRE(data.indices[2] == 2);

    std::remove(OBJ_PATH.c_str());
}

TEST_CASE("ObjLoader accepts an explicit MTL path") {
    // The mtllib reference points at a file that does not exist, but the
    // explicitly passed path should be used instead.
    writeFile(OBJ_PATH,
              "mtllib missing.mtl\n"
              "usemtl blue\n"
              "v 0.0 0.0 0.0\n"
              "v 1.0 0.0 0.0\n"
              "v 0.0 1.0 0.0\n"
              "f 1 2 3\n");
    writeFile(MTL_PATH, "newmtl blue\nKd 0.0 0.0 1.0\n");

    ObjLoader loader;
    MeshData data = loader.Load(OBJ_PATH, MTL_PATH);

    REQUIRE(data.vertices.size() == 3);
    for (const Vertex& v : data.vertices) {
        REQUIRE(approxEqual(v.color, glm::vec3(0.0f, 0.0f, 1.0f)));
    }

    std::remove(OBJ_PATH.c_str());
    std::remove(MTL_PATH.c_str());
}

TEST_CASE("ObjLoader returns empty data for a missing file") {
    ObjLoader loader;
    MeshData data = loader.Load("does_not_exist.obj");

    REQUIRE(data.vertices.empty());
    REQUIRE(data.indices.empty());
}
