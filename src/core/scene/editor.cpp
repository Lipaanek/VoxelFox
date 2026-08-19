#include "editor.hpp"

#include "../scripting/camera/lua_camera.hpp"
#include "../scripting/inputs/lua_input_bindings.hpp"
#include "../scripting/vector/lua_vector3.hpp"
#include "../util/util.hpp"

Editor::Editor(SceneManager& sceneManager, Window& window, InputSystem& inputSystem)
    : sceneManager(sceneManager), window(window), inputSystem(inputSystem)
{
    this->editorScripts_.emplace_back(std::make_unique<LuaScript>(this->editorLuaEngine));
    auto& script = *this->editorScripts_.back();

    const auto success = script.setScript("assets/scripts/camera_inputs.lua", { "editor" });
    Util::Log::scriptLoadLog(success);

    this->editorLuaEngine.addScript(&script);
    
    this->inputSystem.setDefaultBindings();

    LuaInputBindings::registerInput(this->editorLuaEngine.state(), &this->inputSystem);
    LuaVector3Bindings::registerVector3(this->editorLuaEngine.state());
    LuaCameraBindings::registerCamera(this->editorLuaEngine.state(), &this->getCamera());
}

Camera &Editor::getCamera() {
    return this->camera;
}

void Editor::update(const float dt) const {
    editorLuaEngine.runUpdate(dt);
    this->sceneManager.update(dt);
}

void Editor::render(const RenderContext& ctx) const {
    this->sceneManager.render(ctx);
}

void Editor::setScene(std::unique_ptr<Scene> scene) const {
    this->sceneManager.setScene(std::move(scene));
}
