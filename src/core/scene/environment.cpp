#include "environment.hpp"

#include <utility>

#include "imgui.h"
#include "core/input/input_system.hpp"
#include "../renderer/render_context.hpp"
#include "../scene/scene.hpp"
#include "../scene/scene_manager.hpp"

Environment::Environment(EnvironmentObjects objects)
    : m_sceneManager(objects.sceneManager), m_window(objects.window), m_inputSystem(objects.inputSystem),
      m_uiRenderer(objects.window), m_camera(objects.camera), m_uiRenderMode(objects.uiRenderMode),
      m_luaApiRegistry(std::move(objects.luaApiRegistry)) {
    m_luaEngine.registerApiBindings(
        m_luaApiRegistry,
        {
            .camera = &m_camera,
            .inputSystem = &m_inputSystem,
            .inputMapScope = m_uiRenderMode == UIRenderMode::Editor
                ? InputMapScope::Editor
                : InputMapScope::Runtime,
            .eventBus = &m_eventBus
        }
    );
}

Camera& Environment::getCamera() {
    return m_camera;
}

const Camera& Environment::getCamera() const {
    return m_camera;
}

void Environment::update(const float dt) {
    m_sceneManager.update(dt);
}

void Environment::render() {
    m_sceneManager.render();
    if (m_uiRenderMode == UIRenderMode::Editor)
        renderEditorUI();

    if (Scene* scene = m_sceneManager.getCurrentScene())
        m_uiRenderer.renderRuntimeUI(scene->getUIElements(), m_uiRenderMode);

    endUI();
}

void Environment::setScene(std::unique_ptr<Scene> scene) const {
    if (scene)
        scene->lua().registerApiBindings(
            m_luaApiRegistry,
            {
                .camera = &m_camera,
                .inputSystem = &m_inputSystem,
                .eventBus = &scene->eventBus()
            }
        );

    m_sceneManager.setScene(std::move(scene));
}

void Environment::beginUI() {
    const UICaptureState capture = m_uiRenderer.begin();
    m_inputSystem.setUICapture(capture.mouse, capture.keyboard);
}

void Environment::endUI() {
    const UICaptureState capture = m_uiRenderer.end();
    m_inputSystem.setUICapture(capture.mouse, capture.keyboard);
}

void Environment::renderEditorUI() {

}
