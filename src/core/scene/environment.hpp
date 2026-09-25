#pragma once

#include <memory>

#include "core/camera/camera.hpp"
#include "core/scripting/lua_engine.hpp"
#include "core/scripting/lua_api_registry.hpp"

class SceneManager;
class Window;
class InputSystem;
class Scene;
class MeshRenderContext;

struct EnvironmentObjects {
    SceneManager& sceneManager;
    Window& window;
    InputSystem& inputSystem;
    Camera& camera;
    LuaApiRegistry luaApiRegistry = LuaApiRegistry::createDefault();
};

class Environment {
protected:
    SceneManager& sceneManager;
    Window& window;
    InputSystem& inputSystem;

    Camera& camera;
    LuaApiRegistry luaApiRegistry;
    LuaEngine luaEngine;

public:
    explicit Environment(EnvironmentObjects objects);

    virtual ~Environment() = default;

    Camera& getCamera();
    [[nodiscard]] const Camera& getCamera() const;

    virtual void update(float dt);
    virtual void render();

    void setScene(std::unique_ptr<Scene> scene) const;
};