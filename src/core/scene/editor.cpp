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
    m_projectFile{ Util::File("project.vfp"), {} },
    m_projectFileHandler{ m_projectFile }
{
    inputSystem.setEditorDefaultBindings();

    m_editorScripts.emplace_back(
        std::make_unique<LuaScript>(m_luaEngine)
    );

    auto& script = *m_editorScripts.back();

    const auto success = script.setScript(
        "assets/scripts/camera_inputs.lua",
        { "editor" }
    );

    Util::Log::scriptLoadLog(success);

    m_luaEngine.addScript(&script);
}

void Editor::update(const float dt) {
    m_luaEngine.runUpdate(dt);

    // Deleted cus editor doesn't need node scripts to be running
    Environment::update(dt);
}

ProjectFileHandler& Editor::getProjectFileHandler() noexcept {
    return m_projectFileHandler;
}

void Editor::renderEditorUI() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            const auto reportSaveError = [](const std::exception& error) {
                Util::Log::error(std::format("Failed to save project: {}", error.what()));
                tinyfd_messageBox("Save Project Failed", error.what(), "ok", "error", 1);
            };

            const auto reportLoadError = [](const std::exception& error) {
                Util::Log::error(std::format("Failed to load project: {}", error.what()));
                tinyfd_messageBox("Load Project Failed", error.what(), "ok", "error", 1);
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

            const auto loadProject = [this, &reportLoadError] {
                const char* filters[] = { "*.vfp" };
                const char* path = tinyfd_openFileDialog(
                    "Load VoxelFox Project",
                    "",
                    1,
                    filters,
                    "VoxelFox project",
                    0
                );

                if (path == nullptr) return;

                try {
                    m_projectFileHandler.load(path);
                    m_activeProjectPath = path;

                    Util::Log::log(std::format("Loaded project: {}", m_activeProjectPath.string()));
                }
                catch (const std::exception& error) {
                    reportLoadError(error);
                }
            };

            if (ImGui::MenuItem("Save Project")) {
                if (m_activeProjectPath.empty()) {
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

            if (ImGui::MenuItem("Load Project")) {
                loadProject();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    m_uiRenderer.renderEditorDockspace();
}

void Editor::saveProject() const {
    if (m_activeProjectPath.empty()) {
        throw std::logic_error("Cannot save a project before choosing a project path");
    }

    m_projectFileHandler.save();
}

void Editor::saveProjectAs(const std::string& filePath) {
    const std::filesystem::path selectedPath{ filePath };
    const std::string projectName = selectedPath.stem().string();

    if (projectName.empty()) {
        throw std::invalid_argument("Project name cannot be empty");
    }

    const std::filesystem::path projectFolder = selectedPath.parent_path() / projectName;
    const Util::Directory directory { projectFolder };

    ProjectFile pendingProject = m_projectFile;
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

    m_projectFile = std::move(pendingProject);
    m_activeProjectPath = projectFolder / "project.vfp";
}
