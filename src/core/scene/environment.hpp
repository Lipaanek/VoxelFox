#pragma once

#include <memory>

#include "core/camera/camera.hpp"
#include "core/gui/ui_renderer.hpp"
#include "core/scripting/lua_engine.hpp"
#include "core/scripting/lua_api_registry.hpp"
#include "core/util/event_bus.hpp"

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
    UIRenderMode uiRenderMode = UIRenderMode::Runtime;
};

class Environment {
protected:
    SceneManager& sceneManager;
    Window& window;
    InputSystem& inputSystem;
    UIRenderer uiRenderer;

    Camera& camera;
    UIRenderMode uiRenderMode;
    LuaApiRegistry luaApiRegistry;
    Util::EventBus eventBus;
    LuaEngine luaEngine;

public:
    explicit Environment(EnvironmentObjects objects);

    virtual ~Environment() = default;

    Camera& getCamera();
    [[nodiscard]] const Camera& getCamera() const;

    virtual void update(float dt);
    virtual void render();

    void beginUI();
    void endUI();
    void setScene(std::unique_ptr<Scene> scene) const;

protected:
    virtual void renderEditorUI();
};