#pragma once

#include <cstddef>
#include <type_traits>

#include "mesh_manager.hpp"
#include "../render_context.hpp"

#include "../../scene/scene.hpp"
#include "core/renderer/frame_render_data.hpp"

struct MeshRenderInstance {
    glm::mat4 transform;
    glm::vec4 color;
};

static_assert(std::is_standard_layout_v<MeshRenderInstance>);
static_assert(offsetof(MeshRenderInstance, transform) == 0);
static_assert(offsetof(MeshRenderInstance, color) == sizeof(glm::mat4));
static_assert(sizeof(MeshRenderInstance) == sizeof(glm::mat4) + sizeof(glm::vec4));

class MeshRenderer {
private:
    Buffer instanceBuffer { GL_SHADER_STORAGE_BUFFER };
    ShaderProgram program;

public:
    MeshRenderer();

    void render(const FrameRenderData &frameData, Scene &scene);
    void uploadLights(const FrameRenderData& frameData);
};
