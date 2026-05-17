#include <glad/glad.h>
#include <imgui/imgui.h>

#include "create_project_screen.hpp"

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
    ImGui::InputText("##ProjectPath", projectPath, sizeof(projectPath));
    
    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::Button("Create", ImVec2(formWidth, 0))) {
        projectCreated = true;
    }

    ImGui::End();
}
