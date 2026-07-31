#pragma once

#include "../core/screen/screen.hpp"
#include "../core/renderer/mesh/mesh.hpp"
#include "../core/camera/camera.hpp"
#include "../core/window/window.hpp"
#include "../core/renderer/light.hpp"

class BasicScreen : public Screen {
private:
    ShaderProgram& program;
    SceneLights lights;
    Window& window;
    Mesh mesh;
    Camera camera;
    glm::mat4 model { 1.0f };

public:
    BasicScreen(ShaderProgram& program, Window& window) : program(program), window(window) {}

    void render() override;
    void onReady() override;
};
