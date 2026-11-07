#include "window.hpp"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <stdexcept>

Window::Window(const WindowSettings &settings)
    : callbackContext {.window = this, .inputSystem = nullptr }, width(settings.width), height(settings.height) {
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    if (this->width <= 0 || this->height <= 0) {
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        this->width = mode->width;
        this->height = mode->height;
    }

    glfwSetErrorCallback([](const int code, const char* desc) {
        fprintf(stderr, "GLFW error %d: %s\n", code, desc);
    });

    this->handle = glfwCreateWindow(width, height, settings.title, nullptr, nullptr);

    if (!this->handle) {
        glfwTerminate();
        throw std::runtime_error("Failed to initialize window");
    }

    glfwMakeContextCurrent(this->handle);

    glfwWindowHint(GLFW_MAXIMIZED, static_cast<int>(settings.type));
    glfwSetWindowAttrib(this->handle, GLFW_MAXIMIZED, GLFW_TRUE);
    glfwMaximizeWindow(this->handle);

    if (!gladLoadGL())
        throw std::runtime_error("Failed to init glad");

    glfwGetWindowSize(this->handle, &width, &height);
    updateFramebuffer();

    glEnable(GL_DEPTH_TEST);

    // V-Sync
    glfwSwapInterval(static_cast<int>(settings.vsync));

    // Window resize handling
    glfwSetWindowUserPointer(this->handle, &callbackContext);
    glfwSetFramebufferSizeCallback(this->handle, Window::framebufferSizeCallback);
}

Window::~Window() {
    if (this->handle) {
        glfwSetWindowUserPointer(this->handle, nullptr);
        glfwDestroyWindow(this->handle);
        this->handle = nullptr;
    }

    glfwTerminate();
}

void Window::update() {
    glfwPollEvents();
    glfwGetWindowSize(this->handle, &width, &height);
    updateFramebuffer();
}

void Window::updateFramebuffer() {
    glfwGetFramebufferSize(this->handle, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);
}

void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    const auto* context = static_cast<WindowCallbackContext*>(glfwGetWindowUserPointer(window));
    if (!context || !context->window)
        return;

    context->window->windowResize(width, height);
}

void Window::windowResize(int width, int height) {
    this->width = width;
    this->height = height;

    glViewport(0, 0, width, height);
}

// Displays current frame
void Window::present() const {
    glfwSwapBuffers(this->handle);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(this->handle);
}

GLFWwindow* Window::getHandle() const {
    return this->handle;
}

int Window::getWidth() const {
    return this->width;
}

int Window::getHeight() const {
    return this->height;
}

float Window::getAspect() const {
    if (framebufferWidth <= 0 || framebufferHeight <= 0)
        return 1.0f;

    return static_cast<float>(framebufferWidth) / static_cast<float>(framebufferHeight);
}