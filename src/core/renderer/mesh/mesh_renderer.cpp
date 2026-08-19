#include "mesh_renderer.hpp"

#include "../../../nodes/mesh_instance_3d.hpp"
#include "../../util/util.hpp"

MeshRenderer::MeshRenderer(MeshManager &meshManager)
    : meshManager(meshManager) {}

void MeshRenderer::render(const RenderContext& ctx, Scene& scene) const {
    this->uploadLights(ctx, scene);

    ctx.program.setUniform("u_view", ctx.camera.getViewMatrix());
    ctx.program.setUniform("u_projection", ctx.camera.getProjectionMatrix(
        ctx.window.getAspect())
        );

    ctx.program.setUniform("u_cameraPos", ctx.camera.getPosition());

    this->renderNode(ctx, *scene.getRoot());
}

void MeshRenderer::renderNode(const RenderContext& ctx, const Node& node) const
{
    if (const auto* meshInstance =
            dynamic_cast<const MeshInstance3D*>(&node))
    {
        const MeshID meshID = meshInstance->getMesh();

        if (meshID != -1) {
            const Mesh& mesh = this->meshManager.get(meshID);

            ctx.program.setUniform(
                "u_model",
                meshInstance->getGlobalMatrix()
            );

            mesh.draw(ctx.program);
        }
    }

    for (const auto& child : node.getChildren()) {
        this->renderNode(ctx, *child);
    }
}

void MeshRenderer::uploadLights(const RenderContext& ctx, Scene &scene) const {
    Lighting lighting = scene.getLighting();

    lighting.lights.uploadLights(ctx.program);

    ctx.program.setUniform("u_shininess", lighting.getShininess());
    ctx.program.setUniform("u_skyColor", lighting.getSkyColor());
    ctx.program.setUniform("u_groundColor", lighting.getGroundColor());
}
