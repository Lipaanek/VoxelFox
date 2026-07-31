#pragma once

#include <string>
#include "../renderer/mesh/mesh_data.hpp"

class Loader {
public:
    virtual ~Loader() = default;

    virtual MeshData Load(const std::string& filePath, const std::string& mtlPath = "") = 0;
};
