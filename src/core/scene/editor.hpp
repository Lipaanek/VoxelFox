#pragma once

#include "Environment.hpp"
#include "core/readable_files/project_file_handler.hpp"

#include <filesystem>
#include <memory>
#include <vector>

class LuaScript;

class Editor : public Environment {
private:
    std::vector<std::unique_ptr<LuaScript>> editorScripts;
    ProjectFile projectFile;
    ProjectFileHandler projectFileHandler;
    std::filesystem::path activeProjectPath;

public:
    Editor(
        SceneManager& sceneManager,
        Window& window,
        InputSystem& inputSystem,
        Camera& camera
    );

    void update(float dt) override;
    ProjectFileHandler& getProjectFileHandler() noexcept;

protected:
    void renderEditorUI() override;

    void saveProject() const;
    void saveProjectAs(const std::string& filePath);
};