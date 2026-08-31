#pragma once
#include "mesh_manager.hpp"
#include "../render_context.hpp"

#include "../../scene/scene.hpp"

struct RenderInstance {
    glm::mat4 transform;
    glm::vec4 color;
};

class MeshRenderer {
private:
    std::unordered_map<MeshID, std::vector<RenderInstance>> instances;
    Buffer instanceBuffer { GL_SHADER_STORAGE_BUFFER };

public:
    MeshRenderer() = default;

    void render(const RenderContext& ctx, Scene& scene);
    void uploadLights(const RenderContext& ctx, Scene &scene) const;
    void collectMeshes(const RenderContext& ctx, const Node& node, Scene& scene);

};