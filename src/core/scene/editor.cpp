#include "editor.hpp"

#include <format>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <utility>

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
    }),
    projectFile{ Util::File("project.vfp"), {} },
    projectFileHandler{ projectFile }
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

ProjectFileHandler& Editor::getProjectFileHandler() noexcept {
    return projectFileHandler;
}

void Editor::renderEditorUI() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            const auto reportSaveError = [](const std::exception& error) {
                Util::Log::error(std::format("Failed to save project: {}", error.what()));
                tinyfd_messageBox("Save Project Failed", error.what(), "ok", "error", 1);
            };

            const auto saveAs = [this, &reportSaveError] {
                const char* filters[] = { "*.vfp" };
                const char* path = tinyfd_saveFileDialog(
                    "Save VoxelFox Project",
                    "project.vfp",
                    1,
                    filters,
                    "VoxelFox project"
                );

                if (path == nullptr) return;

                try {
                    saveProjectAs(path);
                }
                catch (const std::exception& error) {
                    reportSaveError(error);
                }
            };

            if (ImGui::MenuItem("Save Project")) {
                if (activeProjectPath.empty()) {
                    saveAs();
                }
                else {
                    try {
                        saveProject();
                    }
                    catch (const std::exception& error) {
                        reportSaveError(error);
                    }
                }
            }

            if (ImGui::MenuItem("Save Project As")) {
                saveAs();
            }

            ImGui::MenuItem("Load Project");
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    uiRenderer.renderEditorDockspace();
}

void Editor::saveProject() const {
    if (activeProjectPath.empty()) {
        throw std::logic_error("Cannot save a project before choosing a project path");
    }

    projectFileHandler.save();
}

void Editor::saveProjectAs(const std::string& filePath) {
    const std::filesystem::path selectedPath{ filePath };
    const std::string projectName = selectedPath.stem().string();
    if (projectName.empty()) {
        throw std::invalid_argument("Project name cannot be empty");
    }

    const std::filesystem::path projectFolder = selectedPath.parent_path() / projectName;
    const Util::Directory directory{ projectFolder };

    ProjectFile pendingProject = projectFile;
    pendingProject.file = directory.file("project.vfp");

    const ProjectFileHandler pendingHandler{ pendingProject };
    pendingHandler.addSection("application");
    pendingHandler.addVariable(
        "application",
        "version",
        std::string{ EngineConfig::ENGINE_VERSION }
    );
    pendingHandler.addVariable("application", "project_name", projectName);
    pendingHandler.save();

    projectFile = std::move(pendingProject);
    activeProjectPath = projectFolder / "project.vfp";
}
