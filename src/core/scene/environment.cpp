#include "environment.hpp"

#include "../input/input_system.hpp"
#include "../renderer/render_context.hpp"
#include "../scene/scene.hpp"
#include "../scene/scene_manager.hpp"
#include "../window/window.hpp"

Environment::Environment(
    SceneManager& sceneManager,
    Window& window,
    InputSystem& inputSystem)
    : sceneManager(sceneManager),
      window(window),
      inputSystem(inputSystem)
{}

Camera& Environment::getCamera() {
    return camera;
}

const Camera& Environment::getCamera() const {
    return camera;
}

void Environment::update(const float dt) {
    sceneManager.update(dt);
}

void Environment::render(const RenderContext& ctx) {
    sceneManager.render(ctx);
}

void Environment::setScene(std::unique_ptr<Scene> scene) const {
    sceneManager.setScene(std::move(scene));
}
