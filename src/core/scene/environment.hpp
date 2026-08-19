#pragma once

#include <memory>

#include "core/camera/camera.hpp"
#include "core/scripting/lua_engine.hpp"

class SceneManager;
class Window;
class InputSystem;
class Scene;
class RenderContext;

class Environment {
protected:
    SceneManager& sceneManager;
    Window& window;
    InputSystem& inputSystem;
    Camera camera;
    LuaEngine luaEngine;

public:
    Environment(
        SceneManager& sceneManager,
        Window& window,
        InputSystem& inputSystem
    );

    virtual ~Environment() = default;

    Camera& getCamera();
    [[nodiscard]] const Camera& getCamera() const;

    virtual void update(float dt);
    virtual void render(const RenderContext& ctx);

    void setScene(std::unique_ptr<Scene> scene) const;
};