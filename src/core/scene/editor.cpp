#include "editor.hpp"

#include <memory>

#include "scene_manager.hpp"
#include "core/input/input_system.hpp"
#include "core/scripting/lua_script.hpp"
#include "core/util/util.hpp"

Editor::Editor(
    SceneManager& sceneManager,
    Window& window,
    InputSystem& inputSystem,
    Camera& camera)
    : Environment({.sceneManager = sceneManager, .window = window, .inputSystem = inputSystem, .camera = camera})
{
    inputSystem.setDefaultBindings();

    editorScripts.emplace_back(
        std::make_unique<LuaScript>(luaEngine)
    );

    auto& script = *editorScripts.back();

    const auto success = script.setScript(
        "assets/scripts/camera_inputs.lua",
        { "editor" }
    );

    Util::Log::scriptLoadLog(success);

    luaEngine.addScript(&script);
}

void Editor::update(const float dt) {
    luaEngine.runUpdate(dt);

    // Deleted cus editor doesn't need node scripts to be running
    //Environment::update(dt);
}

void Editor::render() {
    this->sceneManager.render();
}