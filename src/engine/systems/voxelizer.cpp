#include <glm/glm.hpp>
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <limits>

#include <glad/glad.h>

#include "../../include/voxelizer.hpp"
#include "../../include/vertex.hpp"
#include "../../include/shader_loader.hpp"
#include "../../include/triangle.hpp"
#include "../../include/color_loader.hpp"

using std::vector;

constexpr int chunkSize = 16;

struct VoxelGridInfo {
    glm::ivec3 gridSize;
    glm::vec3  gridOrigin;
};

static vector<PrecomputedTriangle> preprocessTriangles(
    const vector<Vertex>& vertices,
    const vector<uint32_t>& indices,
    const vector<int>& triMaterials,
    float voxelSize,
    VoxelGridInfo& outInfo)
{
    size_t triCount = indices.size() / 3;
    vector<PrecomputedTriangle> result(triCount);

    glm::vec3 meshMin(INFINITY), meshMax(-INFINITY);

    size_t writeIdx = 0;
    for (size_t i = 0; i < triCount; ++i) {
        glm::vec3 verts[3] = {
            vertices[indices[i * 3 + 0]].position,
            vertices[indices[i * 3 + 1]].position,
            vertices[indices[i * 3 + 2]].position
        };

        glm::vec3 e0 = verts[1] - verts[0];
        glm::vec3 e1 = verts[2] - verts[0];

        if (glm::length(glm::cross(e0, e1)) < 1e-12f)
            continue;

        auto& t = result[writeIdx++];
        t.v0 = verts[0];
        t.v1 = verts[1];
        t.v2 = verts[2];
        t.e0 = e0;
        t.e1 = verts[2] - verts[1];
        t.e2 = verts[0] - verts[2];
        t.normal = glm::normalize(glm::cross(e0, t.e1));
        t.d = -glm::dot(t.normal, verts[0]);
        t.materialIndex = triMaterials.empty() ? 0 : std::max(0, triMaterials[i]);

        glm::vec3 aabbMin = glm::min(glm::min(verts[0], verts[1]), verts[2]);
        glm::vec3 aabbMax = glm::max(glm::max(verts[0], verts[1]), verts[2]);
        meshMin = glm::min(meshMin, aabbMin);
        meshMax = glm::max(meshMax, aabbMax);
    }
    result.resize(writeIdx);

    outInfo.gridOrigin = meshMin;

    for (size_t i = 0; i < writeIdx; ++i) {
        auto& t = result[i];
        glm::vec3 aabbMin = glm::min(glm::min(t.v0, t.v1), t.v2);
        glm::vec3 aabbMax = glm::max(glm::max(t.v0, t.v1), t.v2);
        glm::vec3 aabbMinRel = aabbMin - outInfo.gridOrigin;
        glm::vec3 aabbMaxRel = aabbMax - outInfo.gridOrigin;
        t.voxelMin = glm::ivec3(glm::floor(aabbMinRel / voxelSize));
        t.voxelMax = glm::ivec3(glm::floor(aabbMaxRel / voxelSize));
    }

    glm::vec3 gridExtent = meshMax - meshMin;
    outInfo.gridSize = glm::ivec3(glm::ceil(gridExtent / voxelSize - 1e-7f));

    printf("Preprocessed %zu triangles, grid %dx%dx%d = %d voxels\n",
        triCount, outInfo.gridSize.x, outInfo.gridSize.y, outInfo.gridSize.z,
        outInfo.gridSize.x * outInfo.gridSize.y * outInfo.gridSize.z);

    return result;
}

vector<VoxelChunk> voxelizeGPUCompute(vector<Vertex> vertices, vector<uint32_t> indices, const vector<int>& triMaterials, float voxelSize) {
    VoxelGridInfo gridInfo;
    auto triangles = preprocessTriangles(vertices, indices, triMaterials, voxelSize, gridInfo);

    size_t triCount = triangles.size();
    if (triCount == 0) {
        printf("No triangles to process\n");
        return {};
    }

    gridInfo.gridSize = ((gridInfo.gridSize + 15) / 16) * 16;

    size_t totalVoxels = (size_t)gridInfo.gridSize.x * gridInfo.gridSize.y * gridInfo.gridSize.z;
    size_t totalBytes = totalVoxels * sizeof(uint32_t);
    printf("Grid aligned to %dx%dx%d = %zu voxels (%.2f MB)\n",
           gridInfo.gridSize.x, gridInfo.gridSize.y, gridInfo.gridSize.z,
           totalVoxels, totalBytes / (1024.0 * 1024.0));

    // Surface pass setup
    GLuint triSSBO;
    glGenBuffers(1, &triSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, triSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
        triCount * sizeof(PrecomputedTriangle),
        triangles.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, triSSBO);

    GLuint voxelSSBO;
    glGenBuffers(1, &voxelSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, voxelSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, totalBytes, nullptr, GL_DYNAMIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, voxelSSBO);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, voxelSSBO);
    std::vector<uint32_t> zeros(totalVoxels, 0);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, totalBytes, zeros.data());

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        printf("OpenGL error after buffer setup: %d\n", err);
        glDeleteBuffers(1, &triSSBO);
        glDeleteBuffers(1, &voxelSSBO);
        return {};
    }

    // Voxel compute shader compilation
    unsigned int computeShader = compileComputeShader("src/shaders/voxel_compute.glsl");
    if (computeShader == 0) {
        printf("Failed to compile surface compute shader\n");
        glDeleteBuffers(1, &triSSBO);
        glDeleteBuffers(1, &voxelSSBO);
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
        printf("Surface compute program link error: %s\n", infoLog);
        glDeleteShader(computeShader);
        glDeleteProgram(computeProgram);
        glDeleteBuffers(1, &triSSBO);
        glDeleteBuffers(1, &voxelSSBO);
        return {};
    }

    glUseProgram(computeProgram);
    GLint gridOriginLoc = glGetUniformLocation(computeProgram, "gridOrigin");
    GLint gridSizeLoc = glGetUniformLocation(computeProgram, "gridSize");
    GLint voxelSizeLoc = glGetUniformLocation(computeProgram, "voxelSize");
    GLint baseIndexLoc = glGetUniformLocation(computeProgram, "baseIndex");

    glUniform3fv(gridOriginLoc, 1, &gridInfo.gridOrigin[0]);
    glUniform3iv(gridSizeLoc, 1, &gridInfo.gridSize[0]);
    glUniform1f(voxelSizeLoc, voxelSize);

    const GLuint batchMax = 65535u;
    GLuint offset = 0;
    while (offset < triCount) {
        GLuint batchSize = (triCount - offset < batchMax) ? static_cast<GLuint>(triCount - offset) : batchMax;
        glUniform1ui(baseIndexLoc, offset);
        printf("  Dispatch batch: offset=%u, count=%u\n", offset, batchSize);
        glDispatchCompute(batchSize, 1, 1);
        offset += batchSize;
    }
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // Readback
    err = glGetError();
    if (err != GL_NO_ERROR) {
        printf("OpenGL error before readback: %d\n", err);
    }

    vector<uint32_t> voxelData(totalVoxels, 0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, voxelSSBO);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, totalBytes, voxelData.data());

    err = glGetError();
    if (err != GL_NO_ERROR) {
        printf("OpenGL error during readback: %d\n", err);
    }

    // Flood fill from grid boundaries

    {
        int sx = gridInfo.gridSize.x;
        int sy = gridInfo.gridSize.y;
        int sz = gridInfo.gridSize.z;
        size_t strideXY = static_cast<size_t>(sx) * static_cast<size_t>(sy);
        size_t strideX = static_cast<size_t>(sx);

        std::vector<glm::ivec3> stack;

        auto seed = [&](int x, int y, int z) {
            size_t idx = static_cast<size_t>(z) * strideXY + static_cast<size_t>(y) * strideX + static_cast<size_t>(x);
            if (voxelData[idx] == 0) {
                voxelData[idx] = 3;
                stack.push_back(glm::ivec3(x, y, z));
            }
        };

        for (int y = 0; y < sy; y++) {
            for (int x = 0; x < sx; x++) {
                seed(x, y, 0);
                if (sz > 1) seed(x, y, sz - 1);
            }
        }
        for (int z = 0; z < sz; z++) {
            for (int x = 0; x < sx; x++) {
                seed(x, 0, z);
                if (sy > 1) seed(x, sy - 1, z);
            }
        }
        for (int z = 0; z < sz; z++) {
            for (int y = 0; y < sy; y++) {
                seed(0, y, z);
                if (sx > 1) seed(sx - 1, y, z);
            }
        }

        static const glm::ivec3 neighbors[6] = {
            {1, 0, 0}, {-1, 0, 0},
            {0, 1, 0}, {0, -1, 0},
            {0, 0, 1}, {0, 0, -1}
        };

        while (!stack.empty()) {
            glm::ivec3 p = stack.back();
            stack.pop_back();

            for (int i = 0; i < 6; i++) {
                int nx = p.x + neighbors[i].x;
                int ny = p.y + neighbors[i].y;
                int nz = p.z + neighbors[i].z;

                if (nx < 0 || nx >= sx || ny < 0 || ny >= sy || nz < 0 || nz >= sz) continue;

                size_t nidx = static_cast<size_t>(nz) * strideXY + static_cast<size_t>(ny) * strideX + static_cast<size_t>(nx);
                if (voxelData[nidx] == 0) {
                    voxelData[nidx] = 3;
                    stack.push_back(glm::ivec3(nx, ny, nz));
                }
            }
        }

        size_t total = static_cast<size_t>(sx) * static_cast<size_t>(sy) * static_cast<size_t>(sz);
        for (size_t i = 0; i < total; i++) {
            if (voxelData[i] == 0) voxelData[i] = 2;
        }
        for (size_t i = 0; i < total; i++) {
            if (voxelData[i] == 3) voxelData[i] = 0;
        }

        printf("Flood fill complete: %zu interior voxels filled\n",
            std::count_if(voxelData.begin(), voxelData.end(), [](uint32_t v) { return v == 2; }));
    }

    // Slicing model into chunks
    int numChunksX = (gridInfo.gridSize.x + chunkSize - 1) / chunkSize;
    int numChunksY = (gridInfo.gridSize.y + chunkSize - 1) / chunkSize;
    int numChunksZ = (gridInfo.gridSize.z + chunkSize - 1) / chunkSize;

    vector<VoxelChunk> chunks;
    chunks.reserve(static_cast<size_t>(numChunksX) * numChunksY * numChunksZ);

    for (int cz = 0; cz < numChunksZ; cz++) {
        for (int cy = 0; cy < numChunksY; cy++) {
            for (int cx = 0; cx < numChunksX; cx++) {
                VoxelChunk chunk;
                chunk.chunkPos = glm::ivec3(cx, cy, cz);
                chunk.chunkSize = glm::ivec3(chunkSize);
                chunk.voxelSize = voxelSize;
                chunk.gridOrigin = gridInfo.gridOrigin;
                chunk.voxels.resize(chunkSize * chunkSize * chunkSize);

                int baseX = cx * chunkSize;
                int baseY = cy * chunkSize;
                int baseZ = cz * chunkSize;

                for (int vz = 0; vz < chunkSize; vz++) {
                    int gz = baseZ + vz;
                    if (gz >= gridInfo.gridSize.z) continue;
                    for (int vy = 0; vy < chunkSize; vy++) {
                        int gy = baseY + vy;
                        if (gy >= gridInfo.gridSize.y) continue;
                        for (int vx = 0; vx < chunkSize; vx++) {
                            int gx = baseX + vx;
                            if (gx >= gridInfo.gridSize.x) continue;

                            uint32_t val = voxelData[
                                static_cast<size_t>(gz) * gridInfo.gridSize.x * gridInfo.gridSize.y +
                                static_cast<size_t>(gy) * gridInfo.gridSize.x +
                                gx];

                            int localIdx = vx + vy * chunkSize + vz * chunkSize * chunkSize;
                            chunk.voxels[localIdx].solid = val != 0;
                            chunk.voxels[localIdx].data.x = static_cast<int>(val);
                        }
                    }
                }

                chunks.push_back(chunk);
            }
        }
    }

    // Cleanup
    glDeleteProgram(computeProgram);
    glDeleteShader(computeShader);
    glDeleteBuffers(1, &triSSBO);
    glDeleteBuffers(1, &voxelSSBO);

    return chunks;
}

VoxelMesh generateVoxelMesh(const vector<VoxelChunk>& chunks, const vector<Material>& materials) {
    VoxelMesh result;
    vector<Vertex> vertices;
    vector<uint32_t> indices;

    static const glm::vec3 faceVerts[6][4] = {
        {{0,0,0}, {1,0,0}, {1,1,0}, {0,1,0}},
        {{0,0,1}, {0,1,1}, {1,1,1}, {1,0,1}},
        {{0,0,0}, {1,0,0}, {1,0,1}, {0,0,1}},
        {{0,1,0}, {1,1,0}, {1,1,1}, {0,1,1}},
        {{0,0,0}, {0,0,1}, {0,1,1}, {0,1,0}},
        {{1,0,0}, {1,0,1}, {1,1,1}, {1,1,0}},
    };
    static const glm::vec3 faceNormals[6] = {
        {0,0,-1}, {0,0,1}, {0,-1,0}, {0,1,0}, {-1,0,0}, {1,0,0},
    };

    for (const auto& chunk : chunks) {
        float vSize = chunk.voxelSize;
        glm::vec3 chunkWorldPos = chunk.gridOrigin + glm::vec3(chunk.chunkPos) * (static_cast<float>(chunkSize) * vSize);

        for (int z = 0; z < chunkSize; ++z) {
            for (int y = 0; y < chunkSize; ++y) {
                for (int x = 0; x < chunkSize; ++x) {
                    int idx = x + y * chunkSize + z * chunkSize * chunkSize;
                    if (!chunk.voxels[idx].solid) continue;

                    int voxelVal = chunk.voxels[idx].data.x;
                    glm::vec3 voxelColor = {0.6f, 0.6f, 0.6f};
                    if ((voxelVal & 1) && !materials.empty()) {
                        int matIdx = voxelVal >> 2;
                        if (matIdx >= 0 && matIdx < static_cast<int>(materials.size())) {
                            voxelColor = {materials[matIdx].color.r, materials[matIdx].color.g, materials[matIdx].color.b};
                        }
                    }

                    glm::vec3 basePos = chunkWorldPos + glm::vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)) * vSize;

                    for (int face = 0; face < 6; ++face) {
                        uint32_t faceStart = static_cast<uint32_t>(vertices.size());
                        for (int v = 0; v < 4; ++v) {
                            glm::vec3 pos = basePos + faceVerts[face][v] * vSize;
                            vertices.push_back({pos, faceNormals[face], {0, 0}, voxelColor});
                        }
                        indices.push_back(faceStart + 0);
                        indices.push_back(faceStart + 1);
                        indices.push_back(faceStart + 2);
                        indices.push_back(faceStart + 0);
                        indices.push_back(faceStart + 2);
                        indices.push_back(faceStart + 3);
                    }
                }
            }
        }
    }

    result.vertices = std::move(vertices);
    result.indices = std::move(indices);
    return result;
}
