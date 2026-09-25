#pragma once

#include "Environment.hpp"

#include <memory>
#include <vector>

class LuaScript;

class Editor : public Environment {
private:
    std::vector<std::unique_ptr<LuaScript>> editorScripts;

public:
    Editor(
        SceneManager& sceneManager,
        Window& window,
        InputSystem& inputSystem,
        Camera& camera
    );

    void update(float dt) override;
    void render() override;
};