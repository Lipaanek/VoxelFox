#include <glm/glm.hpp>
#include <unordered_set>
#include <cmath>

#include "../../headers/voxelizer.hpp"
#include "../../headers/voxel.hpp"
#include "../../headers/vertex.hpp"

using namespace std;

const float voxelSize = 0.1f;

inline glm::ivec3 worldToVoxel(glm::vec3 p) {
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
    const glm::vec3& c)
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
        if (conservativePointInTriangle(corners[i], a, b, c)) {
            return true;
        }
    }
    
    if (segmentVoxelIntersect(a, b, voxel, voxelSize)) return true;
    if (segmentVoxelIntersect(b, c, voxel, voxelSize)) return true;
    if (segmentVoxelIntersect(c, a, voxel, voxelSize)) return true;
    
    return false;
}

vector<Voxel> voxelize(vector<Vertex> vert) {
    vector<Voxel> voxels;
    unordered_set<glm::ivec3, VoxelHash> voxelSet;
    
    for (size_t i = 0; i < vert.size(); i += 3) {
        Vertex v1 = vert[i];
        Vertex v2 = vert[i + 1];
        Vertex v3 = vert[i + 2];
        
        glm::vec3 minP = glm::min(v1.position, glm::min(v2.position, v3.position));
        glm::vec3 maxP = glm::max(v1.position, glm::max(v2.position, v3.position));
        
        glm::ivec3 minVoxel = worldToVoxel(minP - glm::vec3(voxelSize * 0.5f));
        glm::ivec3 maxVoxel = worldToVoxel(maxP + glm::vec3(voxelSize * 0.5f));
        
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