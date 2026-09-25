#pragma once
#include "frame_render_data.hpp"
#include "../camera/camera.hpp"
#include "../window/window.hpp"

class Lighting;

class FrameDataCollector {
public:
    void collectNode(FrameRenderData &data, Node *node);

    static CameraRenderData collectCameraData(const Camera& camera, float aspect);
    static LightingData collectLightingData(const Lighting& lighting);
    FrameRenderData collect(const Scene &scene, const Window &window, const Camera& camera);
};
