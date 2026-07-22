#pragma once

#include "../core/window.hpp"
#include <GLFW/glfw3.h>
#include <memory>
#include "screen.hpp"

class ScreenManager {
private:
    Window &window;
    std::unique_ptr<Screen> currentScreen = nullptr;
public:
    ScreenManager(Window &window);
    ~ScreenManager() = default;

    void setScreen(std::unique_ptr<Screen> screen);
    void update();
    void render();
};
