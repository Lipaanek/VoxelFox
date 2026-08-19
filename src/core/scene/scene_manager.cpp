#include "scene_manager.hpp"

SceneManager::SceneManager(Window& window, MeshRenderer& renderer)
    : window(window), renderer(renderer) {
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

void SceneManager::render(const RenderContext& ctx) const {
    if (this->currentScene)
        renderer.render(ctx, *this->currentScene);
}
