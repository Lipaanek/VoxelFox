#pragma once

#include <memory>
#include <string>

class Scene;

class SceneFormat {
public:
    static bool Save(const Scene& scene, const std::string& filePath);
    static std::unique_ptr<Scene> Load(const std::string& filePath);
};
