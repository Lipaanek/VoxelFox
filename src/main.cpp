#include <memory>

#include "core/window.hpp"
#include "scene/screen_manager.hpp"
#include "scene/basic_screen.cpp"
#include "renderer/shader_program.hpp"

int main() {
    Window window("VoxelFox", 800, 600);

    ScreenManager screenManager(window);

    screenManager.setScreen(std::make_unique<BasicScreen>());

    ShaderProgram program;

    Shader frag("assets/shaders/test_frag.glsl", ShaderType::Fragment);
    Shader vert("assets/shaders/test_vert.glsl", ShaderType::Vertex);

    // Need to compile the shaders before linking
    frag.compile();
    vert.compile();

    program.attach(vert);
    program.attach(frag);
    program.link();

    while (!window.shouldClose()) {
        window.update();
        screenManager.update();

        // Render
        glClear(GL_COLOR_BUFFER_BIT);
        screenManager.render();

        window.present();
    }

    return 0;
}