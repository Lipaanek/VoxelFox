#include "environment.hpp"

#include <utility>

#include "../renderer/render_context.hpp"
#include "../scene/scene.hpp"
#include "../scene/scene_manager.hpp"

Environment::Environment(EnvironmentObjects objects)
    : sceneManager(objects.sceneManager), window(objects.window), inputSystem(objects.inputSystem),
      uiRenderer(objects.window), camera(objects.camera), luaApiRegistry(std::move(objects.luaApiRegistry)) {
    luaEngine.registerApiBindings(
        luaApiRegistry,
        { .camera = &camera, .inputSystem = &inputSystem, .eventBus = &eventBus }
    );
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
    endUI();
}

void Environment::setScene(std::unique_ptr<Scene> scene) const {
    if (scene)
        scene->lua().registerApiBindings(
            luaApiRegistry,
            {
                .camera = &camera,
                .inputSystem = &inputSystem,
                .eventBus = &scene->eventBus()
            }
        );

    sceneManager.setScene(std::move(scene));
}

void Environment::beginUI() {
    this->uiRenderer.begin();
}

void Environment::endUI() {
    uiRenderer.render(uiElements);
}

void Environment::addUIElement(UIElement element) {
    uiElements.push_back(std::move(element));
}

std::vector<UIElement>& Environment::getUIElements() {
    return uiElements;
}

const std::vector<UIElement>& Environment::getUIElements() const {
    return uiElements;
}
