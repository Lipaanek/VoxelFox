#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

enum class VSync {
    VSyncEnabled = 1,
    VSyncDisabled = 0,
};

struct WindowSettings {
    int width;
    int height;
    const char* title;
    VSync vsync;
};

class Window {
private:
    GLFWwindow* handle = nullptr;
    int width;
    int height;

public:
    Window(const WindowSettings &settings);
    ~Window();

    bool shouldClose() const;
    void update();
    void present();

    GLFWwindow* getHandle() const;
    int getWidth() const;
    int getHeight() const;
    float getAspect() const;
};