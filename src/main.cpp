#include <memory>

#include "core/window/window.hpp"
#include "core/screen/screen_manager.hpp"
#include "scene/basic_screen.hpp"
#include "core/renderer/shader_program.hpp"
#include "core/util/util.hpp"

int main() {
    Window window("VoxelFox", 800, 600);

    // Make screen manager
    ScreenManager screenManager(window);

    // Shader program and shader creation
    ShaderProgram program;
    Shader frag("assets/shaders/test_frag.glsl", ShaderType::Fragment);
    Shader vert("assets/shaders/test_vert.glsl", ShaderType::Vertex);

    // Need to compile the shaders before linking
    frag.compile();
    vert.compile();

    if (frag.getID() == 0 || vert.getID() == 0) {
        Util::Log::error("Failed to compile shaders");
        return 1;
    }
        
    // Attach and link programs
    program.attach(vert);
    program.attach(frag);
    program.link();

    screenManager.setScreen(std::make_unique<BasicScreen>(program, window));

    while (!window.shouldClose()) {
        window.update();
        screenManager.update();

        // Clear screen from previous frame
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render
        screenManager.render();

        window.present();
    }

    return 0;
}