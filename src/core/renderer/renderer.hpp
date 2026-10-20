#pragma once
#include "frame_render_data.hpp"
#include "mesh/mesh_renderer.hpp"

class Renderer {
private:
    MeshRenderer meshRenderer {};

public:
    Renderer() = default;
    ~Renderer() = default;

    void render(const FrameRenderData &frd, Scene &scene);
};
