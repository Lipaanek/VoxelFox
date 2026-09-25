#include "frame_data_collector.hpp"

#include "../../nodes/light_3d.hpp"
#include "../../nodes/mesh_instance_3d.hpp"
#include "../camera/camera.hpp"
#include "../scene/scene.hpp"
#include "nodes/ui/ui_panel.hpp"

void FrameDataCollector::collectNode(FrameRenderData& data, Node* node) {
    if (const auto* meshNode = dynamic_cast<MeshInstance3D*>(node)) {
        RenderItem3D item3D {};

        item3D.mesh = meshNode->getMesh();
        item3D.transform = meshNode->getGlobalMatrix();
        item3D.color = {meshNode->getColor(), 1.0f};

        data.objects3D.push_back(item3D);
    } else if (const auto* lightNode = dynamic_cast<Light3D*>(node)) {
        LightItem3D light3D {};

        light3D.light = lightNode->getLightObject();
        light3D.position = lightNode->getPosition();

        data.lights3D.push_back(light3D);
    } else if (const auto* guiPanelNode = dynamic_cast<UIPanel*>(node)) {
        RenderItemGUI guiItem {};

        guiItem.transform.position = guiPanelNode->getPosition();
        guiItem.transform.dimensions = guiPanelNode->getDimensions();
        guiItem.transform.anchor = guiPanelNode->getAnchor();
        guiItem.transform.scale = guiPanelNode->getScale();

        guiItem.color = { guiPanelNode->getColor(), guiPanelNode->getTransparency() };
        guiItem.uiType = UIType::Panel;

        data.gui.push_back(guiItem);
    }

    for (const auto& child : node->getChildren()) {
        this->collectNode(data, child.get());
    }
}

CameraRenderData FrameDataCollector::collectCameraData(const Camera& camera, const float aspect) {
    return {
        .view = camera.getViewMatrix(),
        .projection = camera.getProjectionMatrix(aspect),
        .position = camera.getPosition()
    };
}

LightingData FrameDataCollector::collectLightingData(const Lighting& lighting) {
    return {
        .shininess = lighting.getShininess(),
        .skyColor = lighting.getSkyColor(),
        .groundColor = lighting.getGroundColor()
    };
}

FrameRenderData FrameDataCollector::collect(const Scene& scene, const Window &window, const Camera& camera) {
    FrameRenderData data {};

    data.windowData.width = window.getWidth();
    data.windowData.height = window.getHeight();

    data.camera = collectCameraData(camera, window.getAspect());
    data.lightingData = collectLightingData(scene.getLighting());

    this->collectNode(data, scene.getRoot());

    return data;
}
