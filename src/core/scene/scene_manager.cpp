#include "scene_manager.hpp"

SceneManager::SceneManager(Window& window, MeshRenderer& renderer)
    : window(window), renderer(renderer) {
}

void SceneManager::setScreen(std::unique_ptr<Scene> screen) {
    currentScreen = std::move(screen);
    currentScreen->ready();
}

void SceneManager::update(const float dt) const {
    if (currentScreen)
        currentScreen->update(dt);
}

void SceneManager::render(const RenderContext& ctx) const {
    if (this->currentScreen)
        renderer.render(ctx, *this->currentScreen);
}
