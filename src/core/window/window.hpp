#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

enum class VSync {
    VSyncEnabled = 1,
    VSyncDisabled = 0,
};

class Window;
class InputSystem;

struct WindowCallbackContext {
    Window* window;
    InputSystem* inputSystem = nullptr;
};

struct WindowSettings {
    int width;
    int height;
    const char* title;
    VSync vsync;
};

class Window {
    friend class InputSystem;

private:
    GLFWwindow* handle = nullptr;
    WindowCallbackContext callbackContext;
    int width;
    int height;
    int framebufferWidth = 0;
    int framebufferHeight = 0;

    void updateFramebuffer();

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

public:
    explicit Window(const WindowSettings &settings);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    void update();
    void present() const;

    void windowResize(int width, int height);

    [[nodiscard]] GLFWwindow* getHandle() const;
    [[nodiscard]] int getWidth() const;
    [[nodiscard]] int getHeight() const;
    [[nodiscard]] float getAspect() const;
    [[nodiscard]] bool shouldClose() const;
};