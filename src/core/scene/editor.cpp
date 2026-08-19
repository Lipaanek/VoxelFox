#include "editor.hpp"

#include <memory>

#include "core/input/input_system.hpp"
#include "core/scripting/lua_script.hpp"
#include "core/scripting/camera/lua_camera.hpp"
#include "core/scripting/inputs/lua_input_bindings.hpp"
#include "core/scripting/vector/lua_vector3.hpp"
#include "core/util/util.hpp"

Editor::Editor(
    SceneManager& sceneManager,
    Window& window,
    InputSystem& inputSystem)
    : Environment(sceneManager, window, inputSystem)
{
    editorScripts.emplace_back(
        std::make_unique<LuaScript>(editorLuaEngine)
    );

    auto& script = *editorScripts.back();

    const auto success = script.setScript(
        "assets/scripts/camera_inputs.lua",
        { "editor" }
    );

    Util::Log::scriptLoadLog(success);

    editorLuaEngine.addScript(&script);

    inputSystem.setDefaultBindings();

    LuaInputBindings::registerInput(
        editorLuaEngine.state(),
        &inputSystem
    );

    LuaVector3Bindings::registerVector3(
        editorLuaEngine.state()
    );

    LuaCameraBindings::registerCamera(
        editorLuaEngine.state(),
        &getCamera()
    );
}

void Editor::update(const float dt) {
    editorLuaEngine.runUpdate(dt);

    // Deleted cus editor doesn't need node scripts to be running
    //Environment::update(dt);
}

void Editor::render(const RenderContext& ctx) {
    Environment::render(ctx);
}