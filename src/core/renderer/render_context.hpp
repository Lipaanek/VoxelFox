#pragma once

#include "shader_program.hpp"
#include "../camera/camera.hpp"
#include "../window/window.hpp"
#include "core/util/frustum_culling.hpp"

struct RenderContext {
    ShaderProgram& program;
    Camera& camera;
    Window& window;
    Frustum camFrustum;
};
