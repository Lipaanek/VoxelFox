#include "voxelizer.hpp"

#include "../renderer/buffer.hpp"
#include "../renderer/shader.hpp"
#include "../renderer/shader_program.hpp"
#include "../util/util.hpp"

#include <glad/glad.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <string>
#include <vector>

namespace {

constexpr char kComputeShaderPath[] = "assets/shaders/voxelizer.comp";
constexpr unsigned int kWorkGroupSize = 256;
constexpr size_t kMaxCellCount = 1u << 22;

struct TriData {
    glm::vec4 p0;
    glm::vec4 p1;
    glm::vec4 p2;
    glm::vec4 color;
};

unsigned int ceilDiv(unsigned int numerator, unsigned int denominator) {
    return (numerator + denominator - 1) / denominator;
}

ShaderProgram& computeProgram() {
    static ShaderProgram* program = nullptr;
    if (program == nullptr) {
        auto* comp = new Shader(kComputeShaderPath, ShaderType::Compute);
        program = new ShaderProgram;
        if (comp->compile() == 0) {
            Util::Log::error("Voxelizer: failed to compile compute shader");
            return *program;
        }
        program->attach(*comp);
        program->link();
    }
    return *program;
}

} // namespace

std::vector<GPUVoxel> Voxelizer::voxelize(const MeshData& mesh, float gridSize) {
    if (mesh.vertices.empty() || mesh.indices.size() < 3) {
        Util::Log::error("Voxelizer: mesh has no vertices or triangles");
        return {};
    }

    glm::vec3 minCorner = mesh.vertices[0].position;
    glm::vec3 maxCorner = mesh.vertices[0].position;
    for (const auto& vertex : mesh.vertices) {
        minCorner = glm::min(minCorner, vertex.position);
        maxCorner = glm::max(maxCorner, vertex.position);
    }

    const glm::vec3 extent = maxCorner - minCorner;
    const glm::uvec3 gridDims(
        std::max(1u, static_cast<unsigned int>(std::ceil(extent.x / gridSize))),
        std::max(1u, static_cast<unsigned int>(std::ceil(extent.y / gridSize))),
        std::max(1u, static_cast<unsigned int>(std::ceil(extent.z / gridSize))));

    const unsigned long long totalCells =
        static_cast<unsigned long long>(gridDims.x) * gridDims.y * gridDims.z;
    const size_t cellCount = static_cast<size_t>(std::min<unsigned long long>(totalCells, kMaxCellCount));

    if (totalCells > kMaxCellCount)
        Util::Log::error("Voxelizer: grid too large (" + std::to_string(totalCells)
            + " cells), clamped to " + std::to_string(kMaxCellCount));

    std::vector<TriData> tris;
    tris.reserve(mesh.indices.size() / 3);
    for (size_t i = 0; i + 2 < mesh.indices.size(); i += 3) {
        const Vertex& a = mesh.vertices[mesh.indices[i]];
        const Vertex& b = mesh.vertices[mesh.indices[i + 1]];
        const Vertex& c = mesh.vertices[mesh.indices[i + 2]];
        tris.push_back({glm::vec4(a.position, 1.0f),
                        glm::vec4(b.position, 1.0f),
                        glm::vec4(c.position, 1.0f),
                        glm::vec4((a.color + b.color + c.color) / 3.0f, 1.0f)});
    }

    const unsigned int triCount = static_cast<unsigned int>(tris.size());

    ShaderProgram& program = computeProgram();
    if (program.getID() == 0) {
        Util::Log::error("Voxelizer: compute program unavailable");
        return {};
    }

    const GLint originLoc = glGetUniformLocation(program.getID(), "gridOrigin");
    const GLint dimsLoc = glGetUniformLocation(program.getID(), "gridDims");
    const GLint sizeLoc = glGetUniformLocation(program.getID(), "gridSize");
    const GLint triCountLoc = glGetUniformLocation(program.getID(), "triCount");
    const GLint cellLimitLoc = glGetUniformLocation(program.getID(), "cellLimit");
    if (originLoc == -1 || dimsLoc == -1 || sizeLoc == -1 || triCountLoc == -1 || cellLimitLoc == -1) {
        Util::Log::error("Voxelizer: compute uniforms not found");
        return {};
    }

    Buffer voxelBuf(GL_SHADER_STORAGE_BUFFER);
    voxelBuf.upload(nullptr, cellCount * sizeof(GPUVoxel), GL_DYNAMIC_READ);

    Buffer triBuf(GL_SHADER_STORAGE_BUFFER);
    triBuf.upload(tris.data(), tris.size() * sizeof(TriData), GL_STATIC_DRAW);

    Buffer counterBuf(GL_SHADER_STORAGE_BUFFER);
    constexpr GLuint zeroCount = 0;
    counterBuf.upload(&zeroCount, sizeof(GLuint), GL_DYNAMIC_READ);

    Buffer occupiedBuf(GL_SHADER_STORAGE_BUFFER);
    std::vector<GLuint> unoccupied(cellCount, 0u);
    occupiedBuf.upload(unoccupied.data(), unoccupied.size() * sizeof(GLuint), GL_DYNAMIC_DRAW);

    program.setUniform("gridOrigin", minCorner);
    program.setUniform("gridDims", glm::vec3(gridDims));
    program.setUniform("gridSize", gridSize);
    program.setUniform("triCount", triCount);
    program.setUniform("cellLimit", static_cast<unsigned int>(cellCount));
    program.setStorageBuffer(0, voxelBuf);
    program.setStorageBuffer(1, triBuf);
    program.setStorageBuffer(2, counterBuf);
    program.setStorageBuffer(3, occupiedBuf);

    program.dispatch(ceilDiv(triCount, kWorkGroupSize), 1, 1);

    // glGetBufferSubData below already synchronizes with the GPU, so no glFinish().
    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    std::vector<GLuint> counter(1);
    counterBuf.read(counter);
    const size_t voxelCount = std::min<size_t>(counter[0], cellCount);

    std::vector<GPUVoxel> voxels;
    voxels.resize(voxelCount);
    if (voxelCount > 0)
        voxelBuf.download(voxels.data(), voxelCount * sizeof(GPUVoxel));

    if (voxelCount == 0)
        Util::Log::error("Voxelizer: no surface voxels generated");

    return voxels;
}
