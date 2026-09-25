#include "scene_manager.hpp"

SceneManager::SceneManager(Window& window, Renderer& renderer, Camera& camera)
    : window(window), renderer(renderer), camera(camera) {
}

void SceneManager::setScene(std::unique_ptr<Scene> scene) {
    currentScene = std::move(scene);
    firstSceneRun = true;
}

void SceneManager::update(const float dt) {
    if (firstSceneRun == true) {
        firstSceneRun = false;
        currentScene->ready();
    }

    if (currentScene)
        currentScene->update(dt);
}

void SceneManager::render() {
    if (this->currentScene) {
        const FrameRenderData data = collector.collect(*currentScene, window, camera);
        renderer.render(data, *this->currentScene);
    }
}
