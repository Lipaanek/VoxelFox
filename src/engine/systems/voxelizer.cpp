#include <glm/glm.hpp>
#include <unordered_set>
#include <unordered_map>
#include <cmath>
#include <fstream>
#include <cstdint>
#include <algorithm>
#include <limits>

#include <glad/glad.h>

#include "../../headers/voxelizer.hpp"
#include "../../headers/vertex.hpp"
#include "../../headers/shader_loader.hpp"
#include "../../headers/triangle.hpp"
#include "../../headers/aabb.hpp"
#include "../../headers/chunk.hpp"

using std::vector;
using std::unordered_map;

constexpr int chunkSize = 16;

Triangle getTriangle(
    const vector<Vertex>& vertices,
    const vector<uint32_t>& indices,
    size_t triIndex)
{
    uint32_t i0 = indices[triIndex * 3 + 0];
    uint32_t i1 = indices[triIndex * 3 + 1];
    uint32_t i2 = indices[triIndex * 3 + 2];

    return {
        vertices[i0].position,
        vertices[i1].position,
        vertices[i2].position
    };
}

AABB computeTriangleAABB(const Triangle& t) {
    glm::vec3 minPos = glm::min(
        glm::min(t.p0, t.p1),
        t.p2
    );

    glm::vec3 maxPos = glm::max(
        glm::max(t.p0, t.p1),
        t.p2
    );

    return { minPos, maxPos };
}

glm::ivec3 worldToChunk(glm::vec3 pos, float chunkWorldSize) {
    return glm::floor(pos / chunkWorldSize);
}

glm::vec3 computeTriangleNormal(const Triangle& t) {
    glm::vec3 edge1 = t.p1 - t.p0;
    glm::vec3 edge2 = t.p2 - t.p0;
    return glm::normalize(glm::cross(edge1, edge2));
}

bool pointInTriangle(glm::vec3 p, const Triangle& t) {
    glm::vec3 v0 = t.p1 - t.p0;
    glm::vec3 v1 = t.p2 - t.p0;
    glm::vec3 v2 = p - t.p0;

    float dot00 = glm::dot(v0, v0);
    float dot01 = glm::dot(v0, v1);
    float dot02 = glm::dot(v0, v2);
    float dot11 = glm::dot(v1, v1);
    float dot12 = glm::dot(v1, v2);

    float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
    float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    return (u >= 0) && (v >= 0) && (u + v <= 1);
}

bool voxelIntersectsTriangle(const AABB& voxel, const Triangle& t) {
    glm::vec3 center = (voxel.min + voxel.max) * 0.5f;
    glm::vec3 halfExtents = (voxel.max - voxel.min) * 0.5f;

    glm::vec3 normal = computeTriangleNormal(t);
    glm::vec3 axes[5] = {
        glm::vec3(1, 0, 0),
        glm::vec3(0, 1, 0),
        glm::vec3(0, 0, 1),
        normal,
        glm::normalize(glm::cross(normal, glm::vec3(1, 0, 0)))
    };

    for (int i = 0; i < 5; ++i) {
        glm::vec3 axis = glm::normalize(axes[i]);
        if (glm::length(axis) < 0.0001f) continue;

        float projVoxelMin = glm::dot(center, axis) - glm::dot(halfExtents, glm::abs(axis));
        float projVoxelMax = glm::dot(center, axis) + glm::dot(halfExtents, glm::abs(axis));

        float projTriMin = std::min({
            glm::dot(t.p0, axis),
            glm::dot(t.p1, axis),
            glm::dot(t.p2, axis)
        });
        float projTriMax = std::max({
            glm::dot(t.p0, axis),
            glm::dot(t.p1, axis),
            glm::dot(t.p2, axis)
        });

        if (projVoxelMax < projTriMin || projTriMax < projVoxelMin) {
            return false;
        }
    }

    glm::vec3 testPoints[8] = {
        voxel.min,
        voxel.max,
        glm::vec3(voxel.min.x, voxel.min.y, voxel.max.z),
        glm::vec3(voxel.min.x, voxel.max.y, voxel.min.z),
        glm::vec3(voxel.max.x, voxel.min.y, voxel.min.z),
        glm::vec3(voxel.max.x, voxel.max.y, voxel.min.z),
        glm::vec3(voxel.min.x, voxel.max.y, voxel.max.z),
        glm::vec3(voxel.max.x, voxel.min.y, voxel.max.z)
    };

    for (int i = 0; i < 8; ++i) {
        if (pointInTriangle(testPoints[i], t)) {
            return true;
        }
    }

    return false;
}

vector<VoxelChunk> voxelizeGPU(vector<Vertex> vertices, vector<uint32_t> indices, float voxelSize) {
    float chunkWorldSize = chunkSize * voxelSize;

    unordered_map<ChunkKey, vector<uint32_t>, ChunkKeyHash> chunkTriangles;

    size_t triangleCount = indices.size() / 3;
    vector<Triangle> triangles(triangleCount);
    vector<AABB> triangleBounds(triangleCount);

    for (size_t tri = 0; tri < triangleCount; ++tri) {
        triangles[tri] = getTriangle(vertices, indices, tri);
        triangleBounds[tri] = computeTriangleAABB(triangles[tri]);

        glm::ivec3 minChunk = worldToChunk(triangleBounds[tri].min, chunkWorldSize);
        glm::ivec3 maxChunk = worldToChunk(triangleBounds[tri].max, chunkWorldSize);

        for (int z = minChunk.z; z <= maxChunk.z; ++z) {
            for (int y = minChunk.y; y <= maxChunk.y; ++y) {
                for (int x = minChunk.x; x <= maxChunk.x; ++x) {
                    ChunkKey key{x, y, z};
                    chunkTriangles[key].push_back(static_cast<uint32_t>(tri));
                }
            }
        }
    }

    vector<VoxelChunk> chunks;
    for (auto& [key, triIndices] : chunkTriangles) {
        VoxelChunk chunk;

        chunk.chunkPos = glm::ivec3(key.x, key.y, key.z);
        chunk.chunkSize = glm::ivec3(chunkSize);
        chunk.voxelSize = voxelSize;
        chunk.voxels.resize(chunkSize * chunkSize * chunkSize);

        glm::vec3 chunkOrigin = glm::vec3(key.x, key.y, key.z) * chunkWorldSize;

        for (int z = 0; z < chunkSize; ++z) {
            for (int y = 0; y < chunkSize; ++y) {
                for (int x = 0; x < chunkSize; ++x) {
                    int voxelIndex = x + y * chunkSize + z * chunkSize * chunkSize;

                    glm::vec3 voxelMin = chunkOrigin + glm::vec3(x, y, z) * voxelSize;
                    glm::vec3 voxelMax = voxelMin + glm::vec3(voxelSize);
                    AABB voxelBounds{voxelMin, voxelMax};

                    for (uint32_t triIdx : triIndices) {
                        if (voxelIntersectsTriangle(voxelBounds, triangles[triIdx])) {
                            chunk.voxels[voxelIndex].solid = true;
                            chunk.voxels[voxelIndex].data.x = static_cast<int>(triIdx);
                            break;
                        }
                    }
                }
            }
        }

        chunks.push_back(chunk);
    }
    return chunks;
}

VoxelMesh generateVoxelMesh(const std::vector<VoxelChunk>& chunks) {
    VoxelMesh result;
    std::vector<uint32_t> indices;
    std::vector<Vertex> vertices;
    uint32_t vertexOffset = 0;

    static const glm::vec3 cubeVerts[8] = {
        {0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0},
        {0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}
    };

    static const uint32_t cubeIndices[36] = {
        0, 2, 1, 0, 3, 2,
        4, 5, 6, 4, 6, 7,
        0, 1, 5, 0, 5, 4,
        2, 3, 7, 2, 7, 6,
        0, 4, 7, 0, 7, 3,
        1, 2, 6, 1, 6, 5
    };

    for (const auto& chunk : chunks) {
        float voxelSize = chunk.voxelSize;
        glm::vec3 chunkWorldPos = glm::vec3(chunk.chunkPos) * (chunkSize * voxelSize);

        for (int z = 0; z < chunkSize; ++z) {
            for (int y = 0; y < chunkSize; ++y) {
                for (int x = 0; x < chunkSize; ++x) {
                    int idx = x + y * chunkSize + z * chunkSize * chunkSize;
                    if (!chunk.voxels[idx].solid) continue;

                    glm::vec3 basePos = chunkWorldPos + glm::vec3(x, y, z) * voxelSize;

                    for (int i = 0; i < 8; ++i) {
                        glm::vec3 pos = basePos + cubeVerts[i] * voxelSize;
                        vertices.push_back({pos, {0, 0, 1}, {0, 0}});
                    }

                    for (int i = 0; i < 36; ++i) {
                        indices.push_back(vertexOffset + cubeIndices[i]);
                    }
                    vertexOffset += 8;
                }
            }
        }
    }

    result.vertices = std::move(vertices);
    result.indices = std::move(indices);
    return result;
}

struct TriangleSSBO {
    glm::vec3 p0;
    float _pad0;
    glm::vec3 p1;
    float _pad1;
    glm::vec3 p2;
    float _pad2;
};

std::vector<VoxelChunk> voxelizeGPUCompute(std::vector<Vertex> vertices, std::vector<uint32_t> indices, float voxelSize) {
    unsigned int computeShader = compileComputeShader("src/shaders/voxel_compute.glsl");
    if (computeShader == 0) {
        printf("Failed to compile compute shader\n");
        return {};
    }
    unsigned int computeProgram = glCreateProgram();
    glAttachShader(computeProgram, computeShader);
    glLinkProgram(computeProgram);

    int linkSuccess;
    glGetProgramiv(computeProgram, GL_LINK_STATUS, &linkSuccess);
    if (!linkSuccess) {
        char infoLog[512];
        glGetProgramInfoLog(computeProgram, 512, nullptr, infoLog);
        printf("Compute program link error: %s\n", infoLog);
        return {};
    }

    size_t triangleCount = indices.size() / 3;
    printf("Processing %zu triangles\n", triangleCount);
    if (triangleCount == 0) {
        printf("No triangles to process\n");
        return {};
    }

    printf("About to compile compute shader\n");
    std::vector<TriangleSSBO> triangleSSBO(triangleCount);
    for (size_t i = 0; i < triangleCount; ++i) {
        Triangle t = getTriangle(vertices, indices, i);
        triangleSSBO[i] = {t.p0, 0.0f, t.p1, 0.0f, t.p2, 0.0f};
    }

    GLuint triangleSSBOHandle;
    glGenBuffers(1, &triangleSSBOHandle);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangleSSBOHandle);
    glBufferData(GL_SHADER_STORAGE_BUFFER, triangleCount * sizeof(TriangleSSBO), triangleSSBO.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, triangleSSBOHandle);

    float chunkWorldSize = chunkSize * voxelSize;
    std::unordered_map<ChunkKey, std::vector<uint32_t>, ChunkKeyHash> chunkTriangles;

    for (size_t tri = 0; tri < triangleCount; ++tri) {
        Triangle triangle = getTriangle(vertices, indices, tri);
        AABB bounds = computeTriangleAABB(triangle);

        glm::ivec3 minChunk = worldToChunk(bounds.min, chunkWorldSize);
        glm::ivec3 maxChunk = worldToChunk(bounds.max, chunkWorldSize);

        for (int z = minChunk.z; z <= maxChunk.z; ++z) {
            for (int y = minChunk.y; y <= maxChunk.y; ++y) {
                for (int x = minChunk.x; x <= maxChunk.x; ++x) {
                    ChunkKey key{x, y, z};
                    chunkTriangles[key].push_back(static_cast<uint32_t>(tri));
                }
            }
        }
    }

    std::vector<VoxelChunk> chunks;
    chunks.reserve(chunkTriangles.size());

    std::vector<GLuint> voxelSSBOs;
    glUseProgram(computeProgram);

    printf("Processing %zu chunks\n", chunkTriangles.size());

    for (auto& [key, triIndices] : chunkTriangles) {
        printf("Processing chunk (%d,%d,%d) with %zu triangles\n", key.x, key.y, key.z, triIndices.size());

        if (triIndices.empty()) {
            continue;
        }

        VoxelChunk chunk;
        chunk.chunkPos = glm::ivec3(key.x, key.y, key.z);
        chunk.chunkSize = glm::ivec3(chunkSize);
        chunk.voxelSize = voxelSize;
        chunk.voxels.resize(chunkSize * chunkSize * chunkSize);

        GLuint voxelSSBO;
        glGenBuffers(1, &voxelSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, voxelSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, chunkSize * chunkSize * chunkSize * sizeof(uint32_t), nullptr, GL_DYNAMIC_READ);

        size_t triIdxCount = triIndices.size();
        GLuint triIdxSSBO;
        glGenBuffers(1, &triIdxSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, triIdxSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, triIdxCount * sizeof(uint32_t), triIndices.data(), GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, triIdxSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, voxelSSBO);

        GLint triIdxCountLoc = glGetUniformLocation(computeProgram, "triIndexCount");
        GLint chunkOffsetLoc = glGetUniformLocation(computeProgram, "chunkOffset");
        GLint chunkSizeLoc = glGetUniformLocation(computeProgram, "chunkSize");
        GLint voxelSizeLoc = glGetUniformLocation(computeProgram, "voxelSize");

        printf("Uniform locations: triIndexCount=%d, chunkOffset=%d, chunkSize=%d, voxelSize=%d\n",
               triIdxCountLoc, chunkOffsetLoc, chunkSizeLoc, voxelSizeLoc);

        if (triIdxCountLoc != -1) glUniform1ui(triIdxCountLoc, static_cast<unsigned int>(triIdxCount));
        if (chunkOffsetLoc != -1) glUniform3fv(chunkOffsetLoc, 1, &(glm::vec3(key.x, key.y, key.z) * chunkWorldSize)[0]);
        if (chunkSizeLoc != -1) glUniform1ui(chunkSizeLoc, static_cast<unsigned int>(chunkSize));
        if (voxelSizeLoc != -1) glUniform1f(voxelSizeLoc, voxelSize);

        glDispatchCompute((chunkSize * chunkSize * chunkSize + 63) / 64, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            printf("OpenGL error after dispatch: %d\n", err);
        }

        std::vector<uint32_t> solidVoxels(chunkSize * chunkSize * chunkSize);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, voxelSSBO);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, solidVoxels.size() * sizeof(uint32_t), solidVoxels.data());

        err = glGetError();
        if (err != GL_NO_ERROR) {
            printf("OpenGL error after read: %d\n", err);
        }

        for (size_t i = 0; i < solidVoxels.size(); ++i) {
            chunk.voxels[i].solid = solidVoxels[i] != 0u;
        }

        chunks.push_back(chunk);
        voxelSSBOs.push_back(voxelSSBO);
    }

    glDeleteProgram(computeProgram);
    glDeleteShader(computeShader);
    glDeleteBuffers(1, &triangleSSBOHandle);
    glDeleteBuffers(static_cast<GLsizei>(voxelSSBOs.size()), voxelSSBOs.data());

    return chunks;
}