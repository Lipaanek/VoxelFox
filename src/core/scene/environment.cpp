#include "environment.hpp"

#include <utility>

#include "../renderer/render_context.hpp"
#include "../scene/scene.hpp"
#include "../scene/scene_manager.hpp"

Environment::Environment(EnvironmentObjects objects)
    : sceneManager(objects.sceneManager), window(objects.window), inputSystem(objects.inputSystem),
      camera(objects.camera), luaApiRegistry(std::move(objects.luaApiRegistry)) {
    luaEngine.registerApiBindings(luaApiRegistry, { .camera = &camera, .inputSystem = &inputSystem });
}

Camera& Environment::getCamera() {
    return camera;
}

const Camera& Environment::getCamera() const {
    return camera;
}

void Environment::update(const float dt) {
    sceneManager.update(dt);
}

void Environment::render() {
    sceneManager.render();
}

void Environment::setScene(std::unique_ptr<Scene> scene) const {
    if (scene)
        scene->lua().registerApiBindings(
            luaApiRegistry,
            { .camera = &camera, .inputSystem = &inputSystem }
        );

    sceneManager.setScene(std::move(scene));
}
