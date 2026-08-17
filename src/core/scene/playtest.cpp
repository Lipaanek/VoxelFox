#include "playtest.hpp"

Playtest::Playtest(SceneManager *sceneManager, Window &window)
    : sceneManager(sceneManager), window(window) {}

Camera &Playtest::getCamera() {
    return this->camera;
}

void Playtest::update(const float dt) const {
    this->sceneManager->update(dt);
}

void Playtest::render(const RenderContext &ctx) const {
    this->sceneManager->render(ctx);
}
