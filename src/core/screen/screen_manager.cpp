#include "screen_manager.hpp"

#include <glm/glm.hpp>

ScreenManager::ScreenManager(Window& window)
    : window(window) {}

void ScreenManager::setScreen(std::unique_ptr<Screen> screen) {
    currentScreen = std::move(screen);
    currentScreen->onReady();
}

void ScreenManager::update() {
    if (currentScreen)
        currentScreen->update();
}

void ScreenManager::render() {
    if (currentScreen)
        currentScreen->render();
}