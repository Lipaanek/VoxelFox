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

using std::vector;

const float DEFAULT_VOXEL_SIZE = 1.0f;
const glm::ivec3 CHUNK_SIZE = glm::ivec3(32);

vector<VoxelChunk> voxelizeGPU(vector<Vertex> vertices, float voxelSize) {
    return {};
}