#include <glm/glm.hpp>

#include "../../headers/voxelizer.hpp"
#include "../../headers/voxel.hpp"
#include "../../headers/vertex.hpp"

using namespace std;

const float voxelSize = 0.1f;

// Helper function
glm::ivec3 worldToVoxel(glm::vec3 p) {
    return glm::ivec3(
        floor(p.x / voxelSize),
        floor(p.y / voxelSize),
        floor(p.z / voxelSize)
    );
}

// Helper function to see if voxel overlaps
bool triangleVoxelOverlap(
    const glm::vec3& a,
    const glm::vec3& b,
    const glm::vec3& c,
    const glm::ivec3& voxel,
    float voxelSize) 
{
    glm::vec3 minV = glm::vec3(voxel) * voxelSize;
    glm::vec3 maxV = minV + glm::vec3(voxelSize);

    // sample resolution
    const int STEPS = 5;

    for (int i = 0; i <= STEPS; i++) {
        float u = (float)i / STEPS;

        for (int j = 0; j <= STEPS - i; j++) {
            float v = (float) j / STEPS;
            float w = 1.0f - u - v;

            glm::vec3 p =
                u * a +
                v * b +
                w * c;

            if (p.x >= minV.x && p.x <= maxV.x &&
                p.y >= minV.y && p.y <= maxV.y &&
                p.z >= minV.z && p.z <= maxV.z)
            {
                return true;
            }
        }
    }

    return false;
}

vector<Voxel> voxelize(vector<Vertex> vert) {
    vector<Voxel> voxels;
    for (size_t i = 0; i < vert.size(); i += 3) {
        Vertex v1 = vert[i];
        Vertex v2 = vert[i + 1];
        Vertex v3 = vert[i + 2];

        glm::vec3 minP = glm::min(v1.position, glm::min(v2.position, v3.position));
        glm::vec3 maxP = glm::max(v1.position, glm::max(v2.position, v3.position));

        glm::ivec3 minVoxel = worldToVoxel(minP);
        glm::ivec3 maxVoxel = worldToVoxel(maxP);

        // Loop through the area
        for (int z = minVoxel.z; z <= maxVoxel.z; z++) {
            for (int y = minVoxel.y; y <= maxVoxel.y; y++) {
                for (int x = minVoxel.x; x <= maxVoxel.x; x++) {
                    if (triangleVoxelOverlap(v1.position, v2.position, v3.position,
                         glm::ivec3(x, y, z),
                         voxelSize)) 
                    {
                        Voxel voxel;
                        voxel.position = glm::ivec3(x, y, z);
                        voxel.solid = true;

                        voxels.push_back(voxel);
                    }
                }
            }
        }
    }

    return voxels;
}