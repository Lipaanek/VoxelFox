#include "editor.hpp"

#include <format>
#include <filesystem>
#include <memory>

#include "imgui.h"
#include "scene_manager.hpp"
#include "tinyfiledialogs.h"
#include "core/engine_config.hpp"
#include "core/input/input_system.hpp"
#include "core/scripting/lua_script.hpp"
#include "core/util/util.hpp"

Editor::Editor(
    SceneManager& sceneManager,
    Window& window,
    InputSystem& inputSystem,
    Camera& camera)
    : Environment({
        .sceneManager = sceneManager,
        .window = window,
        .inputSystem = inputSystem,
        .camera = camera,
        .uiRenderMode = UIRenderMode::Editor
    })
{
    inputSystem.setEditorDefaultBindings();

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
    Environment::update(dt);
}

void Editor::renderEditorUI() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save Project")) {
                const char* filters[] = { "*.vfp" };
                const char* path = tinyfd_saveFileDialog(
                    "Save VoxelFox Project",
                    "project.vfp",
                    1,
                    filters,
                    "VoxelFox project"
                );

                if (path != nullptr) {
                    saveProject(path);
                }
            }

            ImGui::MenuItem("Save Project As");
            ImGui::MenuItem("Load Project");
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    uiRenderer.renderEditorDockspace();
}

void Editor::saveProject(const std::string& filePath) {
    const std::filesystem::path selectedPath { filePath };
    const std::filesystem::path projectFolder = selectedPath.parent_path() / selectedPath.stem();

    const Util::Directory directory { projectFolder };
    Util::File projectFile = directory.file("project.vfp");

    projectFile.writeText("; VoxelFox project\n");
    projectFile.appendText(
        std::format("version=\"{}\"\n", EngineConfig::ENGINE_VERSION)
    );
}
