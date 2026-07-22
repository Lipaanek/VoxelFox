#pragma once

#include <GLFW/glfw3.h>

class Window {
private:
    GLFWwindow* handle = nullptr;

public:
    Window(const char* title, int width, int height);
    ~Window();

    bool shouldClose() const;
    void update();
    void present();
};