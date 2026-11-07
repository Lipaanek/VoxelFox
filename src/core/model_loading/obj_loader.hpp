#pragma once

#include "loader.hpp"

class ObjLoader : public Loader {
public:
    MeshData load(const std::string& filePath, const std::string& mtlPath = "") override;
};
