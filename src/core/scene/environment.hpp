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
    SceneManager& m_sceneManager;
    Window& m_window;
    InputSystem& m_inputSystem;
    UIRenderer m_uiRenderer;

    Camera& m_camera;
    UIRenderMode m_uiRenderMode;
    LuaApiRegistry m_luaApiRegistry;
    Util::EventBus m_eventBus;
    LuaEngine m_luaEngine;

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