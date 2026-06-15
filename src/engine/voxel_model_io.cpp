#include "voxel_model_io.hpp"
#include <glm/glm.hpp>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <cstdio>
#include <cstring>

bool VoxelModelIO::SaveVoxelModel(const std::string& filepath, 
                                 const std::vector<VoxelChunk>& chunks) {
    if (chunks.empty()) {
        std::cerr << "Cannot save empty voxel model" << std::endl;
        return false;
    }

    // Validate that all chunks have the same voxelSize and gridOrigin
    const float referenceVoxelSize = chunks[0].voxelSize;
    const glm::vec3 referenceGridOrigin = chunks[0].gridOrigin;
    for (const auto& chunk : chunks) {
        if (chunk.voxelSize != referenceVoxelSize) {
            std::cerr << "Inconsistent voxelSize across chunks" << std::endl;
            return false;
        }
        if (chunk.gridOrigin != referenceGridOrigin) {
            std::cerr << "Inconsistent gridOrigin across chunks" << std::endl;
            return false;
        }
    }

    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filepath << std::endl;
        return false;
    }

    // Write magic number
    const char magic[4] = {'V', 'O', 'X', 'F'};
    file.write(magic, 4);
    if (!file) {
        std::cerr << "Failed to write magic number" << std::endl;
        file.close();
        return false;
    }

    // Write version
    uint32_t version = 1;
    file.write(reinterpret_cast<const char*>(&version), sizeof(version));
    if (!file) {
        std::cerr << "Failed to write version" << std::endl;
        file.close();
        return false;
    }

    // Write voxel size
    file.write(reinterpret_cast<const char*>(&referenceVoxelSize), sizeof(referenceVoxelSize));
    if (!file) {
        std::cerr << "Failed to write voxel size" << std::endl;
        file.close();
        return false;
    }

    // Write grid origin
    file.write(reinterpret_cast<const char*>(&referenceGridOrigin), sizeof(referenceGridOrigin));
    if (!file) {
        std::cerr << "Failed to write grid origin" << std::endl;
        file.close();
        return false;
    }

    // Write number of chunks
    uint32_t numChunks = static_cast<uint32_t>(chunks.size());
    file.write(reinterpret_cast<const char*>(&numChunks), sizeof(numChunks));
    if (!file) {
        std::cerr << "Failed to write number of chunks" << std::endl;
        file.close();
        return false;
    }

    // Write each chunk
    for (const auto& chunk : chunks) {
        // Write chunk position
        file.write(reinterpret_cast<const char*>(&chunk.chunkPos), sizeof(chunk.chunkPos));
        if (!file) {
            std::cerr << "Failed to write chunk position" << std::endl;
            file.close();
            return false;
        }

        // Write chunk size
        file.write(reinterpret_cast<const char*>(&chunk.chunkSize), sizeof(chunk.chunkSize));
        if (!file) {
            std::cerr << "Failed to write chunk size" << std::endl;
            file.close();
            return false;
        }

        // Write voxel count
        size_t voxelCount = chunk.voxels.size();
        file.write(reinterpret_cast<const char*>(&voxelCount), sizeof(voxelCount));
        if (!file) {
            std::cerr << "Failed to write voxel count" << std::endl;
            file.close();
            return false;
        }

        // Write voxel data array (only the data field, solid is derived)
        if (voxelCount > 0) {
            // Pack into flat ivec4 array to handle sizeof(Voxel) > sizeof(ivec4)
            std::vector<glm::ivec4> flatData(voxelCount);
            for (size_t i = 0; i < voxelCount; i++) {
                flatData[i] = chunk.voxels[i].data;
            }
            file.write(reinterpret_cast<const char*>(flatData.data()),
                       voxelCount * sizeof(glm::ivec4));
            if (!file) {
                std::cerr << "Failed to write voxel data" << std::endl;
                file.close();
                return false;
            }
        }
    }

    file.close();
    return true;
}

std::vector<VoxelChunk> VoxelModelIO::LoadVoxelModel(const std::string& filepath) {
    std::vector<VoxelChunk> result;

    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for reading: " << filepath << std::endl;
        return result;
    }

    // Read and validate magic number
    char magic[4];
    file.read(magic, 4);
    if (!file || memcmp(magic, "VOXF", 4) != 0) {
        std::cerr << "Invalid magic number in voxel file" << std::endl;
        file.close();
        return result;
    }

    // Read version
    uint32_t version;
    file.read(reinterpret_cast<char*>(&version), sizeof(version));
    if (!file) {
        std::cerr << "Failed to read version" << std::endl;
        file.close();
        return result;
    }
    if (version != 1) {
        std::cerr << "Unsupported voxel file version: " << version << std::endl;
        file.close();
        return result;
    }

    // Read voxel size
    float voxelSize;
    file.read(reinterpret_cast<char*>(&voxelSize), sizeof(voxelSize));
    if (!file) {
        std::cerr << "Failed to read voxel size" << std::endl;
        file.close();
        return result;
    }

    // Read grid origin
    glm::vec3 gridOrigin;
    file.read(reinterpret_cast<char*>(&gridOrigin), sizeof(gridOrigin));
    if (!file) {
        std::cerr << "Failed to read grid origin" << std::endl;
        file.close();
        return result;
    }

    // Read number of chunks
    uint32_t numChunks;
    file.read(reinterpret_cast<char*>(&numChunks), sizeof(numChunks));
    if (!file) {
        std::cerr << "Failed to read number of chunks" << std::endl;
        file.close();
        return result;
    }

    // Read each chunk
    for (uint32_t i = 0; i < numChunks; ++i) {
        VoxelChunk chunk;

        // Read chunk position
        file.read(reinterpret_cast<char*>(&chunk.chunkPos), sizeof(chunk.chunkPos));
        if (!file) {
            std::cerr << "Failed to read chunk position" << std::endl;
            file.close();
            return result;
        }

        // Read chunk size
        file.read(reinterpret_cast<char*>(&chunk.chunkSize), sizeof(chunk.chunkSize));
        if (!file) {
            std::cerr << "Failed to read chunk size" << std::endl;
            file.close();
            return result;
        }

        // Set voxel size and grid origin (shared across all chunks)
        chunk.voxelSize = voxelSize;
        chunk.gridOrigin = gridOrigin;

        // Read voxel count
        size_t voxelCount;
        file.read(reinterpret_cast<char*>(&voxelCount), sizeof(voxelCount));
        if (!file) {
            std::cerr << "Failed to read voxel count" << std::endl;
            file.close();
            return result;
        }

        // Read voxel data array (only the data field)
        if (voxelCount > 0) {
            // Read into flat ivec4 array, then unpack into Voxel structs
            std::vector<glm::ivec4> flatData(voxelCount);
            file.read(reinterpret_cast<char*>(flatData.data()),
                      voxelCount * sizeof(glm::ivec4));
            if (!file) {
                std::cerr << "Failed to read voxel data" << std::endl;
                file.close();
                return result;
            }

            chunk.voxels.resize(voxelCount);
            for (size_t i = 0; i < voxelCount; i++) {
                chunk.voxels[i].data = flatData[i];
                chunk.voxels[i].solid = (flatData[i].x != 0);
            }
        }

        result.push_back(chunk);
    }

    file.close();
    return result;
}