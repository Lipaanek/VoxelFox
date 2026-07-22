#include "screen_manager.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

ScreenManager::ScreenManager(Window& window)
    : window(window) {}

void ScreenManager::setScreen(std::unique_ptr<Screen> screen) {
    currentScreen = std::move(screen);
}

void ScreenManager::update() {
    if (currentScreen)
        currentScreen->update();
}

void ScreenManager::render() {
    if (currentScreen)
        currentScreen->render();
}