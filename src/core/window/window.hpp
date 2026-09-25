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
    int framebufferWidth = 0;
    int framebufferHeight = 0;

    void updateFramebuffer();

public:
    explicit Window(const WindowSettings &settings);
    ~Window();

    void update();
    void present();

    [[nodiscard]] GLFWwindow* getHandle() const;
    [[nodiscard]] int getWidth() const;
    [[nodiscard]] int getHeight() const;
    [[nodiscard]] float getAspect() const;
    [[nodiscard]] bool shouldClose() const;
};