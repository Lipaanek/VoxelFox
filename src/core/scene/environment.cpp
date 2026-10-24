#include "environment.hpp"

#include <utility>

#include "core/input/input_system.hpp"
#include "../renderer/render_context.hpp"
#include "../scene/scene.hpp"
#include "../scene/scene_manager.hpp"

Environment::Environment(EnvironmentObjects objects)
    : sceneManager(objects.sceneManager), window(objects.window), inputSystem(objects.inputSystem),
      uiRenderer(objects.window), camera(objects.camera), uiRenderMode(objects.uiRenderMode),
      luaApiRegistry(std::move(objects.luaApiRegistry)) {
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
    if (uiRenderMode == UIRenderMode::Editor)
        renderEditorUI();

    if (Scene* scene = sceneManager.getCurrentScene())
        uiRenderer.renderRuntimeUI(scene->getUIElements(), uiRenderMode);

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
    const UICaptureState capture = uiRenderer.begin();
    inputSystem.setUICapture(capture.mouse, capture.keyboard);
}

void Environment::endUI() {
    const UICaptureState capture = uiRenderer.end();
    inputSystem.setUICapture(capture.mouse, capture.keyboard);
}

void Environment::renderEditorUI() {
}
