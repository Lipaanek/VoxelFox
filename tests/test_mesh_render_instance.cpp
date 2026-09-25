#include "catch_amalgamated.hpp"

#include <cstddef>

#include "core/renderer/mesh/mesh_renderer.hpp"

TEST_CASE("Mesh render instance matches the shader storage buffer layout") {
    REQUIRE(offsetof(MeshRenderInstance, transform) == 0);
    REQUIRE(offsetof(MeshRenderInstance, color) == sizeof(glm::mat4));
    REQUIRE(sizeof(MeshRenderInstance) == sizeof(glm::mat4) + sizeof(glm::vec4));
}
