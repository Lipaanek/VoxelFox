#pragma once

#include "../window/window.hpp"
#include <memory>
#include "scene.hpp"
#include "../renderer/mesh/mesh_renderer.hpp"

class SceneManager {
private:
    Window& window;
    std::unique_ptr<Scene> currentScreen = nullptr;
    MeshRenderer& renderer;

public:
    explicit SceneManager(Window& window, MeshRenderer& renderer);
    ~SceneManager() = default;

    void setScreen(std::unique_ptr<Scene> screen);
    void update(float dt) const;
    void render(const RenderContext& ctx) const;
};
