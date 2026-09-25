#pragma once
#include "frame_render_data.hpp"
#include "mesh/mesh_renderer.hpp"
#include "ui/ui_renderer.hpp"

class Renderer {
private:
    MeshRenderer meshRenderer {};
    UIRenderer uiRenderer {};

public:
    Renderer() = default;
    ~Renderer() = default;

    void render(const FrameRenderData &frd, Scene &scene);
};
