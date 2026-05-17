#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include <glm/glm.hpp>
#include <cstdint>

struct Triangle {
    glm::vec3 p0;
    glm::vec3 p1;
    glm::vec3 p2;
};

struct PrecomputedTriangle {
    glm::vec3 v0;           //  0-11
    float _pad0;            // 12-15
    glm::vec3 v1;           // 16-27
    float _pad1;            // 28-31
    glm::vec3 v2;           // 32-43
    float _pad2;            // 44-47
    glm::vec3 e0;           // 48-59
    float _pad3;            // 60-63
    glm::vec3 e1;           // 64-75
    float _pad4;            // 76-79
    glm::vec3 e2;           // 80-91
    float _pad5;            // 92-95
    glm::vec3 normal;       // 96-107
    float d;                // 108-111
    glm::ivec3 voxelMin;    // 112-123
    float _pad_voxelMin;    // 124-127
    glm::ivec3 voxelMax;    // 128-139
    int materialIndex;      // 140-143
};

static_assert(sizeof(PrecomputedTriangle) == 144,
    "PrecomputedTriangle must be exactly 144 bytes for std430");

#endif