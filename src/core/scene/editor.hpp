#pragma once

#include "scene_manager.hpp"
#include "../camera/camera.hpp"
#include "../input/input_system.hpp"
#include "../scripting/lua_engine.hpp"
#include "../window/window.hpp"

class Editor {
private:
    SceneManager& sceneManager;
    Window& window;
    Camera camera;
    LuaEngine editorLuaEngine;
    InputSystem inputSystem;

public:
    Editor(SceneManager& sceneManager, Window& window, InputSystem& inputSystem);

    Camera &getCamera();

    void update(float dt) const;
    void render(const RenderContext& ctx) const;
    void setScene(std::unique_ptr<Scene> screen) const;
};
