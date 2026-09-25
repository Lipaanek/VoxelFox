#pragma once

#include "shader_program.hpp"
#include "../camera/camera.hpp"
#include "../window/window.hpp"

struct MeshRenderContext {
    ShaderProgram& shader;
    Camera& camera;
    Window& window;
};
