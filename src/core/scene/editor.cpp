#include "editor.hpp"

Editor::Editor(SceneManager *sceneManager, Window &window)
    : sceneManager(sceneManager), window(window) {}

Camera &Editor::getCamera() {
    return this->camera;
}

void Editor::update(const float dt) const {
    this->sceneManager->update(dt);
}

void Editor::render(const RenderContext& ctx) const {
    this->sceneManager->render(ctx);
}
