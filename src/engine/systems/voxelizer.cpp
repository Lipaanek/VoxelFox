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
#include "../../headers/voxel.hpp"
#include "../../headers/vertex.hpp"
#include "../../headers/shader_loader.hpp"

using namespace std;

const float DEFAULT_VOXEL_SIZE = 1.0f;
const glm::ivec3 CHUNK_SIZE = glm::ivec3(32);

inline glm::ivec3 worldToVoxel(glm::vec3 p, float voxelSize) {
    return glm::ivec3(
        static_cast<int>(floor(p.x / voxelSize)),
        static_cast<int>(floor(p.y / voxelSize)),
        static_cast<int>(floor(p.z / voxelSize))
    );
}

static bool pointInTriangleBarycentric(
    const glm::vec3& p,
    const glm::vec3& a,
    const glm::vec3& b,
    const glm::vec3& c)
{
    glm::vec3 v0 = b - a;
    glm::vec3 v1 = c - a;
    glm::vec3 v2 = p - a;
    
    float d00 = glm::dot(v0, v0);
    float d01 = glm::dot(v0, v1);
    float d11 = glm::dot(v1, v1);
    float d20 = glm::dot(v2, v0);
    float d21 = glm::dot(v2, v1);
    
    float denom = d00 * d11 - d01 * d01;
    if (std::abs(denom) < 1e-10f) {
        return false;
    }
    
    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;
    
    return u >= 0 && v >= 0 && w >= 0;
}

static bool conservativePointInTriangle(
    const glm::vec3& p,
    const glm::vec3& a,
    const glm::vec3& b,
    const glm::vec3& c,
    float voxelSize)
{
    return pointInTriangleBarycentric(p, a, b, c) ||
           pointInTriangleBarycentric(p + glm::vec3(voxelSize, 0, 0), a, b, c) ||
           pointInTriangleBarycentric(p - glm::vec3(voxelSize, 0, 0), a, b, c) ||
           pointInTriangleBarycentric(p + glm::vec3(0, voxelSize, 0), a, b, c) ||
           pointInTriangleBarycentric(p - glm::vec3(0, voxelSize, 0), a, b, c) ||
           pointInTriangleBarycentric(p + glm::vec3(0, 0, voxelSize), a, b, c) ||
           pointInTriangleBarycentric(p - glm::vec3(0, 0, voxelSize), a, b, c);
}

static bool segmentVoxelIntersect(
    const glm::vec3& s,
    const glm::vec3& e,
    const glm::ivec3& voxel,
    float voxelSize)
{
    glm::vec3 minV = glm::vec3(voxel) * voxelSize;
    glm::vec3 maxV = minV + glm::vec3(voxelSize);
    
    glm::vec3 d = e - s;
    glm::vec3 f = glm::vec3(1.0f) / d;
    
    glm::vec3 t1 = f * (minV - s);
    glm::vec3 t2 = f * (maxV - s);
    
    glm::vec3 tmin = glm::min(t1, t2);
    glm::vec3 tmax = glm::max(t1, t2);
    
    float tEnter = glm::max(glm::max(tmin.x, tmin.y), tmin.z);
    float tExit = glm::min(glm::min(tmax.x, tmax.y), tmax.z);
    
    return tEnter <= tExit && tExit >= 0 && tEnter <= 1;
}

static bool triangleVoxelOverlapConservative(
    const glm::vec3& a,
    const glm::vec3& b,
    const glm::vec3& c,
    const glm::ivec3& voxel,
    float voxelSize)
{
    glm::vec3 minV = glm::vec3(voxel) * voxelSize;
    glm::vec3 maxV = minV + glm::vec3(voxelSize);
    
    glm::vec3 corners[8] = {
        minV,
        glm::vec3(maxV.x, minV.y, minV.z),
        glm::vec3(minV.x, maxV.y, minV.z),
        glm::vec3(minV.x, minV.y, maxV.z),
        glm::vec3(maxV.x, maxV.y, minV.z),
        glm::vec3(maxV.x, minV.y, maxV.z),
        glm::vec3(minV.x, maxV.y, maxV.z),
        maxV
    };
    
    for (int i = 0; i < 8; i++) {
        if (pointInTriangleBarycentric(corners[i], a, b, c)) {
            return true;
        }
    }
    
    for (int i = 0; i < 8; i++) {
        if (conservativePointInTriangle(corners[i], a, b, c, voxelSize)) {
            return true;
        }
    }
    
    if (segmentVoxelIntersect(a, b, voxel, voxelSize)) return true;
    if (segmentVoxelIntersect(b, c, voxel, voxelSize)) return true;
    if (segmentVoxelIntersect(c, a, voxel, voxelSize)) return true;
    
    return false;
}

vector<Voxel> voxelize(vector<Vertex> vert, float voxelSize) {
    vector<Voxel> voxels;
    unordered_set<glm::ivec3, VoxelHash> voxelSet;
    
    for (size_t i = 0; i < vert.size(); i += 3) {
        Vertex v1 = vert[i];
        Vertex v2 = vert[i + 1];
        Vertex v3 = vert[i + 2];
        
        glm::vec3 minP = glm::min(v1.position, glm::min(v2.position, v3.position));
        glm::vec3 maxP = glm::max(v1.position, glm::max(v2.position, v3.position));
        
        glm::ivec3 minVoxel = worldToVoxel(minP - glm::vec3(voxelSize * 0.5f), voxelSize);
        glm::ivec3 maxVoxel = worldToVoxel(maxP + glm::vec3(voxelSize * 0.5f), voxelSize);
        
        for (int z = minVoxel.z; z <= maxVoxel.z; z++) {
            for (int y = minVoxel.y; y <= maxVoxel.y; y++) {
                for (int x = minVoxel.x; x <= maxVoxel.x; x++) {
                    if (triangleVoxelOverlapConservative(v1.position, v2.position, v3.position,
                         glm::ivec3(x, y, z), voxelSize)) 
                    {
                        glm::ivec3 voxelPos(x, y, z);
                        
                        if (voxelSet.find(voxelPos) == voxelSet.end()) {
                            voxelSet.insert(voxelPos);
                            Voxel voxel;
                            voxel.position = voxelPos;
                            voxel.solid = true;
                            voxels.push_back(voxel);
                        }
                    }
                }
            }
        }
    }
    
    return voxels;
}

static vector<VoxelChunk> groupIntoSections(const vector<Voxel>& voxels, float voxelSize) {
    vector<VoxelChunk> chunks;
    unordered_map<glm::ivec3, size_t, VoxelHash> chunkMap;
    
    for (const Voxel& v : voxels) {
        glm::ivec3 chunkPos = v.position / CHUNK_SIZE;
        
        auto it = chunkMap.find(chunkPos);
        if (it == chunkMap.end()) {
            VoxelChunk chunk;
            chunk.chunkPos = chunkPos;
            chunk.chunkSize = CHUNK_SIZE;
            chunk.voxelSize = voxelSize;
            size_t idx = chunks.size();
            chunks.push_back(chunk);
            chunkMap[chunkPos] = idx;
        }
        
        size_t chunkIdx = chunkMap[chunkPos];
        Voxel localVoxel;
        localVoxel.position = v.position - chunks[chunkIdx].chunkPos * CHUNK_SIZE;
        localVoxel.solid = true;
        chunks[chunkIdx].voxels.push_back(localVoxel);
    }
    
    return chunks;
}

vector<VoxelChunk> voxelizeGPU(vector<Vertex> vertices, float voxelSize) {
    if (vertices.empty()) return {};
    
    glm::vec3 boundsMin(std::numeric_limits<float>::max());
    glm::vec3 boundsMax(-std::numeric_limits<float>::max());
    for (const auto& v : vertices) {
        //boundsMin = glm::min(boundsMin, v.position);
        //boundsMax = glm::max(boundsMax, v.position);
    }
    boundsMin -= glm::vec3(voxelSize * 0.5f);
    boundsMax += glm::vec3(voxelSize * 0.5f);
    
    unsigned int gridSizeX = static_cast<unsigned int>(ceil((boundsMax.x - boundsMin.x) / voxelSize)) + 1;
    unsigned int gridSizeY = static_cast<unsigned int>(ceil((boundsMax.y - boundsMin.y) / voxelSize)) + 1;
    unsigned int gridSizeZ = static_cast<unsigned int>(ceil((boundsMax.z - boundsMin.z) / voxelSize)) + 1;
    
    gridSizeX = std::max(gridSizeX, 1u);
    gridSizeY = std::max(gridSizeY, 1u);
    gridSizeZ = std::max(gridSizeZ, 1u);
    printf("x %i", gridSizeX);
    printf("y %i", gridSizeY);
    printf("z %i", gridSizeZ);
    
    string computeSource = loadFile("src/shaders/voxel_compute.glsl");
    unsigned int computeShader = glCreateShader(GL_COMPUTE_SHADER);
    const char* src = computeSource.c_str();
    glShaderSource(computeShader, 1, &src, nullptr);
    glCompileShader(computeShader);
    
    int success;
    glGetShaderiv(computeShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(computeShader, 512, nullptr, infoLog);
        printf("Compute shader compile error: %s\n", infoLog);
        glDeleteShader(computeShader);
        return {};
    }
    
    unsigned int computeProgram = glCreateProgram();
    glAttachShader(computeProgram, computeShader);
    glLinkProgram(computeProgram);
    glDeleteShader(computeShader);
    
    glGetProgramiv(computeProgram, GL_LINK_STATUS, &success);
    if (!success) {
        printf("Compute shader link error\n");
        glDeleteProgram(computeProgram);
        return {};
    }
    
    unsigned int meshSSBO;
    glGenBuffers(1, &meshSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, meshSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    
    size_t flagCount = static_cast<size_t>(gridSizeX) * gridSizeY * gridSizeZ;
    vector<uint32_t> voxelFlags(flagCount, 0);
    
    unsigned int voxelFlagSSBO;
    glGenBuffers(1, &voxelFlagSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, voxelFlagSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, flagCount * sizeof(uint32_t), nullptr, GL_STATIC_DRAW);
    
    glUseProgram(computeProgram);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, meshSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, voxelFlagSSBO);
    
    glUniform1ui(glGetUniformLocation(computeProgram, "vertexCount"), static_cast<unsigned int>(vertices.size()));
    glUniform1f(glGetUniformLocation(computeProgram, "voxelSize"), voxelSize);
    glUniform3f(glGetUniformLocation(computeProgram, "boundsMin"), boundsMin.x, boundsMin.y, boundsMin.z);
    glUniform3f(glGetUniformLocation(computeProgram, "boundsMax"), boundsMax.x, boundsMax.y, boundsMax.z);
    glUniform1i(glGetUniformLocation(computeProgram, "gridSizeX"), gridSizeX);
    glUniform1i(glGetUniformLocation(computeProgram, "gridSizeY"), gridSizeY);
    glUniform1i(glGetUniformLocation(computeProgram, "gridSizeZ"), gridSizeZ);
    
    glDispatchCompute((static_cast<unsigned int>(vertices.size() / 3) + 63) / 64, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, voxelFlagSSBO);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, flagCount * sizeof(uint32_t), voxelFlags.data());
    
    vector<Voxel> voxels;
    for (size_t z = 0; z < gridSizeZ; z++) {
        for (size_t y = 0; y < gridSizeY; y++) {
            for (size_t x = 0; x < gridSizeX; x++) {
                size_t idx = z * gridSizeX * gridSizeY + y * gridSizeX + x;
                if (voxelFlags[idx] != 0) {
                    Voxel voxel;
                    voxel.position = glm::ivec3(
                        static_cast<int>(x + floor(boundsMin.x / voxelSize)),
                        static_cast<int>(y + floor(boundsMin.y / voxelSize)),
                        static_cast<int>(z + floor(boundsMin.z / voxelSize))
                    );
                    voxel.solid = true;
                    voxels.push_back(voxel);
                }
            }
        }
    }
    
    glDeleteBuffers(1, &meshSSBO);
    glDeleteBuffers(1, &voxelFlagSSBO);
    glDeleteProgram(computeProgram);
    
    return groupIntoSections(voxels, voxelSize);
}

#pragma pack(push, 1)
struct ChunkFileHeader {
    uint32_t magic;
    uint32_t version;
    uint32_t chunkCount;
};
#pragma pack(pop)

void saveChunks(const vector<VoxelChunk>& chunks, const string& path) {
    ofstream file(path, ios::binary);
    if (!file.is_open()) {
        printf("Failed to open file for writing: %s\n", path.c_str());
        return;
    }
    
    ChunkFileHeader header;
    header.magic = 0x46584F56;
    header.version = 1;
    header.chunkCount = chunks.size();
    
    file.write(reinterpret_cast<const char*>(&header), sizeof(header));
    
    for (const VoxelChunk& chunk : chunks) {
        file.write(reinterpret_cast<const char*>(&chunk.chunkPos), sizeof(glm::ivec3));
        uint32_t voxelCount = chunk.voxels.size();
        file.write(reinterpret_cast<const char*>(&voxelCount), sizeof(uint32_t));
        
        for (const Voxel& voxel : chunk.voxels) {
            file.write(reinterpret_cast<const char*>(&voxel.position), sizeof(glm::ivec3));
        }
    }
    
    file.close();
}

vector<VoxelChunk> loadChunks(const string& path) {
    vector<VoxelChunk> chunks;
    ifstream file(path, ios::binary);
    if (!file.is_open()) {
        printf("Failed to open file for reading: %s\n", path.c_str());
        return chunks;
    }
    
    ChunkFileHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));
    
    if (header.magic != 0x46584F56 || header.version != 1) {
        printf("Invalid chunk file format\n");
        return chunks;
    }
    
    chunks.reserve(header.chunkCount);
    
    for (uint32_t i = 0; i < header.chunkCount; i++) {
        VoxelChunk chunk;
        
        file.read(reinterpret_cast<char*>(&chunk.chunkPos), sizeof(glm::ivec3));
        chunk.chunkSize = CHUNK_SIZE;
        
        uint32_t voxelCount;
        file.read(reinterpret_cast<char*>(&voxelCount), sizeof(uint32_t));
        
        chunk.voxels.resize(voxelCount);
        for (uint32_t j = 0; j < voxelCount; j++) {
            file.read(reinterpret_cast<char*>(&chunk.voxels[j].position), sizeof(glm::ivec3));
            chunk.voxels[j].solid = true;
        }
        
        chunks.push_back(chunk);
    }
    
    file.close();
    return chunks;
}