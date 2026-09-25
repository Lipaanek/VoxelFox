#include "mesh_renderer.hpp"

#include "../../../nodes/mesh_instance_3d.hpp"
#include "../../util/util.hpp"

MeshRenderer::MeshRenderer() : program {} {
    Shader frag {"assets/shaders/3d_scene.frag", ShaderType::Fragment};
    Shader vert{"assets/shaders/3d_scene.vert", ShaderType::Vertex};

    frag.compile();
    vert.compile();

    if (frag.getID() == 0 || vert.getID() == 0) {
        Util::Log::error("Failed to compile shaders for 3D rendering.");
        return;
    }

    program.attach(vert);
    program.attach(frag);
    program.link();
}

void MeshRenderer::render(const FrameRenderData& frameData, Scene& scene) {
    this->program.use();

    this->uploadLights(frameData);

    this->program.setUniform("u_view", frameData.camera.view);
    this->program.setUniform("u_projection", frameData.camera.projection);

    this->program.setUniform("u_cameraPos", frameData.camera.position);

    std::unordered_map<MeshID, std::vector<MeshRenderInstance>> instances;

    // Group 3D objects by meshes
    for (const auto&[mesh, transform, color] : frameData.objects3D) {
        instances[mesh].push_back({
            .transform = transform,
            .color = color
        });
    }

    // Use instancing to render meshes
    for (const auto& [meshID, renderInstances] : instances) {
        if (renderInstances.empty())
            continue;

        const Mesh& mesh = scene.getMeshManager().get(meshID);

        instanceBuffer.upload(
            renderInstances.data(),
            static_cast<GLsizeiptr>(
                renderInstances.size() * sizeof(MeshRenderInstance)
            ),
            GL_DYNAMIC_DRAW
        );

        program.setStorageBuffer(0, instanceBuffer);

        mesh.renderInstanced(
            program,
            static_cast<GLsizei>(renderInstances.size())
        );
    }
}

void MeshRenderer::uploadLights(const FrameRenderData& frameData) {
    this->program.setUniform("u_lightCount", static_cast<int>(frameData.lights3D.size()));

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

    this->program.setUniform("u_lightTypes", types);
    this->program.setUniform("u_lightPositions", positions);
    this->program.setUniform("u_lightDirections", directions);
    this->program.setUniform("u_lightColors", colors);
    this->program.setUniform("u_lightEnergy", energies);
    this->program.setUniform("u_lightRanges", ranges);

    this->program.setUniform("u_shininess", frameData.lightingData.shininess);
    this->program.setUniform("u_skyColor", frameData.lightingData.skyColor);
    this->program.setUniform("u_groundColor", frameData.lightingData.groundColor);
}