#include "mesh_renderer.hpp"

#include "../../../nodes/mesh_instance_3d.hpp"
#include "../../util/util.hpp"
#include "../../util/frustum_culling.hpp"

void MeshRenderer::render(const RenderContext& ctx, Scene& scene) {
    this->uploadLights(ctx, scene);

    ctx.program.setUniform("u_view", ctx.camera.getViewMatrix());
    ctx.program.setUniform("u_projection", ctx.camera.getProjectionMatrix(
        ctx.window.getAspect())
    );

    ctx.program.setUniform("u_cameraPos", ctx.camera.getPosition());

    this->collectMeshes(ctx, *scene.getRoot(), scene);

    for (const auto& [meshID, renderInstances] : this->instances) {
        if (renderInstances.empty()) continue;

        const Mesh& mesh = scene.getMeshManager().get(meshID);

        this->instanceBuffer.upload(
            renderInstances.data(),
            static_cast<GLsizeiptr>(
                renderInstances.size() *
                sizeof(RenderInstance)
            ),
            GL_DYNAMIC_DRAW
        );

        ctx.program.setStorageBuffer(0, instanceBuffer);

        mesh.renderInstanced(
            ctx.program,
            static_cast<GLsizei>(
                renderInstances.size()
            )
        );
    }

    this->instances.clear();
}

void MeshRenderer::collectMeshes(const RenderContext& ctx, const Node& node, Scene& scene) {
    auto& chunkManager = scene.getChunkManager();
    const auto visibleChunks = chunkManager.getVisibleChunks(ctx.camFrustum);

    for (const auto& chunkData : visibleChunks) {
        for (auto* meshInstance : chunkData.objects) {
            if (const MeshID meshID = meshInstance->getMesh(); meshID != static_cast<MeshID>(-1)) {
                const glm::mat4 transform = meshInstance->getGlobalMatrix();

                if (test_sphere_against_frustum(ctx.camFrustum, meshInstance->getGlobalBoundingSphere())) {
                    RenderInstance instance {};
                    instance.transform = transform;
                    instance.color = glm::vec4(meshInstance->getColor(), 1.0);
                    this->instances[meshID].push_back(instance);
                }
            }
        }
    }

    collectUnchunked(ctx, node, scene, chunkManager);
}

void MeshRenderer::collectUnchunked(const RenderContext& ctx, const Node& node, Scene& scene, const ChunkManager& chunkManager) {
    if (const auto* meshInstance = dynamic_cast<const MeshInstance3D*>(&node)) {
        if (const MeshID meshID = meshInstance->getMesh(); meshID != static_cast<MeshID>(-1)) {
            if (!chunkManager.isRegistered(meshInstance)) {
                const glm::mat4 transform = meshInstance->getGlobalMatrix();

                if (test_sphere_against_frustum(ctx.camFrustum, meshInstance->getGlobalBoundingSphere())) {
                    RenderInstance instance {};
                    instance.transform = transform;
                    instance.color = glm::vec4(meshInstance->getColor(), 1.0);
                    this->instances[meshID].push_back(instance);
                }
            }
        }
    }

    for (const auto& child : node.getChildren()) {
        collectUnchunked(ctx, *child, scene, chunkManager);
    }
}

void MeshRenderer::uploadLights(const RenderContext& ctx, Scene &scene) const {
    const Lighting lighting = scene.getLighting();

    lighting.lights.uploadLights(ctx.program);

    ctx.program.setUniform("u_shininess", lighting.getShininess());
    ctx.program.setUniform("u_skyColor", lighting.getSkyColor());
    ctx.program.setUniform("u_groundColor", lighting.getGroundColor());
}
