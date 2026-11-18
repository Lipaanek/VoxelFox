#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

enum class VSync {
    VSyncEnabled = 1,
    VSyncDisabled = 0,
};

enum class DisplayType {
    WindowMaximized = 1,
    WindowNormal = 0
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
    DisplayType type;
};

class Window {
    friend class InputSystem;

private:
    GLFWwindow* m_handle = nullptr;
    WindowCallbackContext m_callbackContext;
    int m_width;
    int m_height;
    int m_framebufferWidth = 0;
    int m_framebufferHeight = 0;

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