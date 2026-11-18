#include "scene_manager.hpp"

SceneManager::SceneManager(Window& window, Renderer& renderer, Camera& camera)
    : m_window(window), m_renderer(renderer), m_camera(camera) {
}

void SceneManager::setScene(std::unique_ptr<Scene> scene) {
    m_currentScene = std::move(scene);
    m_firstSceneRun = true;
}

Scene* SceneManager::getCurrentScene() const {
    return m_currentScene.get();
}

void SceneManager::update(const float dt) {
    if (m_firstSceneRun == true) {
        m_firstSceneRun = false;
        m_currentScene->ready();
    }

    if (m_currentScene)
        m_currentScene->update(dt);
}

void SceneManager::render() {
    if (this->m_currentScene) {
        const FrameRenderData data = m_collector.collect(*m_currentScene, m_window, m_camera);
        m_renderer.render(data, *this->m_currentScene);
    }
}
