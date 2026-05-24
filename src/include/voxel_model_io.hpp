#ifndef VOXEL_MODEL_IO_HPP
#define VOXEL_MODEL_IO_HPP

#include "voxel.hpp"
#include <vector>
#include <string>

class VoxelModelIO {
public:
    static bool SaveVoxelModel(const std::string& filepath, 
                              const std::vector<VoxelChunk>& chunks);
    static std::vector<VoxelChunk> LoadVoxelModel(const std::string& filepath);
};

#endif // VOXEL_MODEL_IO_HPP