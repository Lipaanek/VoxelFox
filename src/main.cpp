#include <GLFW/glfw3.h>
#include <memory>

#include "core/window.hpp"
#include "scene/screen_manager.hpp"
#include "scene/basic_screen.cpp"

int main() {
    Window window("VoxelFox", 800, 600);

    ScreenManager screenManager(window);

    screenManager.setScreen(std::make_unique<BasicScreen>());

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