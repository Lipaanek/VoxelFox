#pragma once

#include "../window/window.hpp"
#include <memory>
#include "scene.hpp"
#include "../renderer/mesh/mesh_renderer.hpp"

class SceneManager {
private:
    Window& window;
    std::unique_ptr<Scene> currentScene = nullptr;
    MeshRenderer& renderer;
    bool firstSceneRun = false;

public:
    explicit SceneManager(Window& window, MeshRenderer& renderer);
    ~SceneManager() = default;

    void setScene(std::unique_ptr<Scene> scene);
    void update(float dt);
    void render(const RenderContext& ctx) const;
};
