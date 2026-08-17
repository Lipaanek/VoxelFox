#pragma once

#include "shader_program.hpp"
#include "../camera/camera.hpp"
#include "../window/window.hpp"

struct RenderContext {
    ShaderProgram& program;
    Camera& camera;
    Window& window;
};
