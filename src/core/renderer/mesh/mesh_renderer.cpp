#include "mesh_renderer.hpp"

#include "../../../nodes/mesh_instance_3d.hpp"
#include "../../util/util.hpp"
#include "core/resources/resource.hpp"
#include "core/resources/resource_types/mesh_resource.hpp"

MeshRenderer::MeshRenderer() : m_program {} {
    Shader frag {"assets/shaders/3d_scene.frag", ShaderType::Fragment};
    Shader vert{"assets/shaders/3d_scene.vert", ShaderType::Vertex};

    frag.compile();
    vert.compile();

    if (frag.getID() == 0 || vert.getID() == 0) {
        Util::Log::error("Failed to compile shaders for 3D rendering.");
        return;
    }

    m_program.attach(vert);
    m_program.attach(frag);
    m_program.link();
}

void MeshRenderer::render(const FrameRenderData& frameData, Scene& scene) {
    this->m_program.use();

    this->uploadLights(frameData);

    this->m_program.setUniform("u_view", frameData.camera.view);
    this->m_program.setUniform("u_projection", frameData.camera.projection);

    this->m_program.setUniform("u_cameraPos", frameData.camera.position);

    std::unordered_map<ResourceID, std::vector<MeshRenderInstance>> instances;

    // Group 3D objects by meshes
    for (const auto& [meshID, transform, color] : frameData.objects3D) {
        if (meshID == 0) {
            Util::Log::error("MeshRenderer: mesh instance has no resource assigned");
            continue;
        }

        instances[meshID].push_back({
            .transform = transform,
            .color = color
        });
    }

    // Use instancing to render meshes
    for (const auto& [resourceId, renderInstances] : instances) {
        if (renderInstances.empty())
            continue;

        const auto mesh = scene.getResourceManager().getResource<MeshResource>(resourceId);

        if (!mesh) {
            Util::Log::error("MeshRenderer: mesh resource not found");
            continue;
        }

        m_instanceBuffer.upload(
            renderInstances.data(),
            static_cast<GLsizeiptr>(
                renderInstances.size() * sizeof(MeshRenderInstance)
            ),
            GL_DYNAMIC_DRAW
        );

        m_program.setStorageBuffer(0, m_instanceBuffer);

        m_program.use();
        mesh->getVAO().bind();

        glDrawElementsInstanced(
            GL_TRIANGLES,
            mesh->getIndexCount(),
            GL_UNSIGNED_INT,
            nullptr,
            static_cast<GLsizei>(renderInstances.size())
        );
    }
}

void MeshRenderer::uploadLights(const FrameRenderData& frameData) {
    this->m_program.setUniform("u_lightCount", static_cast<int>(frameData.lights3D.size()));

    std::vector<int> types;
    std::vector<glm::vec3> positions, directions, colors;
    std::vector<float> energies, ranges;

    for (const auto&[light, position] : frameData.lights3D) {
        types.push_back(light.type == LightType::Directional ? 0 : 1);
        positions.push_back(position);
        directions.push_back(light.direction);
        colors.push_back(light.color);
        energies.push_back(light.energy);
        ranges.push_back(light.range);
    }

    this->m_program.setUniform("u_lightTypes", types);
    this->m_program.setUniform("u_lightPositions", positions);
    this->m_program.setUniform("u_lightDirections", directions);
    this->m_program.setUniform("u_lightColors", colors);
    this->m_program.setUniform("u_lightEnergy", energies);
    this->m_program.setUniform("u_lightRanges", ranges);

    this->m_program.setUniform("u_shininess", frameData.lightingData.shininess);
    this->m_program.setUniform("u_skyColor", frameData.lightingData.skyColor);
    this->m_program.setUniform("u_groundColor", frameData.lightingData.groundColor);
}