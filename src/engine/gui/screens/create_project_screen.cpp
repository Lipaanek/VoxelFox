#include <glad/glad.h>
#include <imgui/imgui.h>
#include <filesystem>

#include "create_project_screen.hpp"
#include "../../../engine/project_config.hpp"
#include "../../../utils/file_rule.hpp"
#include <tinyfiledialogs/tinyfiledialogs.h>

void createProject(const std::string& projectName, const std::string& projectPath) {
    FileRule structure =
        FileRule::Dir(projectName)
        .Children({
            FileRule::Dir("assets")
            .Children({
                FileRule::Dir("objects"),
                FileRule::Dir("objectColors"),
                FileRule::Dir("audio"),
                FileRule::Dir("scripts")
            }),

            FileRule::File("project.vfp", [projectName]() {
                return std::string(
                    "# VoxelFox engine project configuration file\n"
                    "# Do not edit unless you know what you are doing\n\n"
                    "engine_version = \"1\"\n"
                    "project_name = \""
                ) + projectName + "\"\n"
                  "voxel_size = \"0.1\"\n";
            })
        });

    structure.Build(projectPath);

    std::string fullProjectPath = (std::filesystem::path(projectPath) / projectName).string();
    ProjectConfig config = ProjectConfigLoader::Load(fullProjectPath);
}

void CreateProjectScreen::Update() {
    if (projectCreated) {
        projectCreated = false;
        if (OnProjectCreated) {
            OnProjectCreated();
        }
    }
}

void CreateProjectScreen::Render() {
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGuiIO& io = ImGui::GetIO();

    ImGui::SetNextWindowSize(ImVec2(420, 180), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f),
        ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));

    ImGui::Begin("Create Project", nullptr);

    float formWidth = ImGui::GetContentRegionAvail().x;

    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Project Name");
    ImGui::InputText("##ProjectName", projectName, sizeof(projectName));

    ImGui::Spacing();

    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Project Path");
    if (ImGui::Button("Browse")) {
        const char* path = tinyfd_selectFolderDialog(
            "Select Project Folder",
            ""
        );

        if (path) {
            strncpy(projectPath, path, sizeof(projectPath));
        }
    }
    
    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::Button("Create", ImVec2(formWidth, 0))) {
        projectCreated = true;

        createProject(std::string(projectName), std::string(projectPath));
    }

    ImGui::End();
}
