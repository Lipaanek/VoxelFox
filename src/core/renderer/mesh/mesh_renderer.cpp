#include "mesh_renderer.hpp"

#include "../../../nodes/mesh_instance_3d.hpp"
#include "../../util/util.hpp"

void MeshRenderer::render(const RenderContext& ctx, Scene& scene) const {
    this->uploadLights(ctx, scene);

    ctx.program.setUniform("u_view", ctx.camera.getViewMatrix());
    ctx.program.setUniform("u_projection", ctx.camera.getProjectionMatrix(
        ctx.window.getAspect())
        );

    ctx.program.setUniform("u_cameraPos", ctx.camera.getPosition());

    this->renderNode(ctx, *scene.getRoot(), scene);
}

void MeshRenderer::renderNode(const RenderContext& ctx, const Node& node, Scene& scene) const
{
    if (const auto* meshInstance =
            dynamic_cast<const MeshInstance3D*>(&node))
    {
        const MeshID meshID = meshInstance->getMesh();

        if (meshID != static_cast<MeshID>(-1)) {
            const Mesh& mesh = scene.getMeshManager().get(meshID);

            ctx.program.setUniform(
                "u_model",
                meshInstance->getGlobalMatrix()
            );

            mesh.render(ctx.program);
        }
    }

    for (const auto& child : node.getChildren()) {
        this->renderNode(ctx, *child, scene);
    }
}

void MeshRenderer::uploadLights(const RenderContext& ctx, Scene &scene) const {
    Lighting lighting = scene.getLighting();

    lighting.lights.uploadLights(ctx.program);

    ctx.program.setUniform("u_shininess", lighting.getShininess());
    ctx.program.setUniform("u_skyColor", lighting.getSkyColor());
    ctx.program.setUniform("u_groundColor", lighting.getGroundColor());
}