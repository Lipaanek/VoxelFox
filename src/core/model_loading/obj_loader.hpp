#pragma once

#include "loader.hpp"

class ObjLoader : public Loader {
public:
    MeshData Load(const std::string& filePath, const std::string& mtlPath = "") override;
};
