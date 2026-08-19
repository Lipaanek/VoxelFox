#pragma once

#include "Environment.hpp"

#include <memory>
#include <vector>

class LuaScript;
class LuaEngine;

class Editor : public Environment {
private:
    LuaEngine editorLuaEngine;
    std::vector<std::unique_ptr<LuaScript>> editorScripts;

public:
    Editor(
        SceneManager& sceneManager,
        Window& window,
        InputSystem& inputSystem
    );

    void update(float dt) override;
    void render(const RenderContext& ctx) override;
};