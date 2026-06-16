#include "light_3d.hpp"
#include <imgui/imgui.h>

void Light3D::BuildUI() {
    Node3D::BuildUI();

    ImGui::Separator();
    float col[3] = {color.x, color.y, color.z};
    if (ImGui::ColorEdit3("Color", col)) {
        color = glm::vec3(col[0], col[1], col[2]);
    }
    ImGui::DragFloat("Intensity", &intensity, 0.1f, 0.0f, 100.0f);
}
