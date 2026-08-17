#pragma once
#include "mesh_manager.hpp"
#include "../render_context.hpp"

#include "../../scene/scene.hpp"

class Scene;

class MeshRenderer {
private:
    MeshManager& meshManager;

public:
    explicit MeshRenderer(MeshManager& meshManager);

    void render(const RenderContext& ctx, Scene& scene) const;
    void uploadLights(const RenderContext& ctx, Scene &scene) const;
    void renderNode(const RenderContext& ctx, const Node& node) const;
};
