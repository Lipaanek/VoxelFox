#pragma once

#include "../window/window.hpp"
#include <memory>
#include "scene.hpp"
#include "core/renderer/frame_data_collector.hpp"
#include "core/renderer/renderer.hpp"

class SceneManager {
private:
    Window& m_window;
    std::unique_ptr<Scene> m_currentScene = nullptr;
    Renderer& m_renderer;
    Camera& m_camera;
    FrameDataCollector m_collector {};
    bool m_firstSceneRun = false;

public:
    explicit SceneManager(Window& window, Renderer& renderer, Camera& camera);
    ~SceneManager() = default;

    void setScene(std::unique_ptr<Scene> scene);
    [[nodiscard]] Scene* getCurrentScene() const;
    void update(float dt);

    void render();

    void render() const;
};
