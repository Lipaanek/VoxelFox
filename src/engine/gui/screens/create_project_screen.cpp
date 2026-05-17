#include <imgui/imgui.h>

#include "create_project_screen.hpp"

void CreateProjectScreen::Update() {
    // Update logic
}

void CreateProjectScreen::Render() {
    ImGui::Begin("Create new project");
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open", "Ctrl+O")) {}
            if (ImGui::MenuItem("New", "Ctrl+N"))  {}
            if (ImGui::MenuItem("Exit", "Ctrl+Q")) {}
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Project Name");
    ImGui::InputText("##ProjectName", projectName, sizeof(projectName));

    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Project Path");
    ImGui::InputText("##ProjectPath", projectPath, sizeof(projectPath));

    ImGui::Button("Create");

    ImGui::End();
}