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

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);

    ImGui::Begin("Create Project", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);

    float formWidth = 400.0f;
    float formHeight = 150.0f;
    ImGui::SetCursorPos(ImVec2(
        (io.DisplaySize.x - formWidth) * 0.5f,
        (io.DisplaySize.y - formHeight) * 0.5f));

    ImGui::BeginGroup();
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
    ImGui::EndGroup();

    ImGui::End();
}
