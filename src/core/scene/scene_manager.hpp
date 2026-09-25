#pragma once

#include "../window/window.hpp"
#include <memory>
#include "scene.hpp"
#include "../renderer/mesh/mesh_renderer.hpp"
#include "core/renderer/frame_data_collector.hpp"
#include "core/renderer/renderer.hpp"

class SceneManager {
private:
    Window& window;
    std::unique_ptr<Scene> currentScene = nullptr;
    Renderer& renderer;
    Camera& camera;
    FrameDataCollector collector {};
    bool firstSceneRun = false;

public:
    explicit SceneManager(Window& window, Renderer& renderer, Camera& camera);
    ~SceneManager() = default;

    void setScene(std::unique_ptr<Scene> scene);
    void update(float dt);

    void render();

    void render() const;
};
