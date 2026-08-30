#pragma once
#include "mesh_manager.hpp"
#include "../render_context.hpp"

#include "../../scene/scene.hpp"

class MeshRenderer {
public:
    MeshRenderer() = default;

    void render(const RenderContext& ctx, Scene& scene) const;
    void uploadLights(const RenderContext& ctx, Scene &scene) const;
    void renderNode(const RenderContext& ctx, const Node& node, Scene& scene) const;
};