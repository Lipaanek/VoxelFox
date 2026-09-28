#pragma once

#include "mesh/mesh_manager.hpp"
#include <glm/glm.hpp>

#include "../../nodes/ui/transform2d.hpp"
#include "../../nodes/ui/ui_node.hpp"
#include "../lighting/light.hpp"

struct CameraRenderData {
    glm::mat4 view { 1.0f };
    glm::mat4 projection { 1.0f };
    glm::vec3 position { 0.0f };
};

struct RenderItem3D {
    MeshID mesh;
    glm::mat4 transform { 1.0f };
    glm::vec4 color { 1.0f };
};

struct RenderItemGUI {
    Transform2D transform;
    glm::vec4 color;
    UIType uiType;
};

struct LightItem3D {
    Light light;
    glm::vec3 position;
};

struct LightingData {
    float shininess = 32.0f;
    glm::vec3 skyColor { 0.3f, 0.35f, 0.45f };
    glm::vec3 groundColor { 0.08f, 0.08f, 0.10f };
};

struct WindowData {
    int width;
    int height;
};

struct FrameRenderData {
    CameraRenderData camera;
    LightingData lightingData;
    WindowData windowData;

    std::vector<RenderItem3D> objects3D;
    std::vector<LightItem3D> lights3D;
    std::vector<RenderItemGUI> gui;
};