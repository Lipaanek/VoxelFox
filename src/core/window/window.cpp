#include "window.hpp"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <stdexcept>

Window::Window(const WindowSettings &settings) : width(settings.width), height(settings.height) {
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

    glfwMakeContextCurrent(handle);

    if (!gladLoadGL())
        throw std::runtime_error("Failed to init glad");

    glfwGetWindowSize(handle, &width, &height);
    updateFramebuffer();

    glEnable(GL_DEPTH_TEST);

    // V-Sync
    glfwSwapInterval(static_cast<int>(settings.vsync));

    // Window resize handling
    glfwSetWindowUserPointer(handle, this);
    glfwSetFramebufferSizeCallback(handle, Window::framebufferSizeCallback);
}

Window::~Window() {
    if (handle)
        glfwDestroyWindow(handle);

    glfwTerminate();
}

void Window::update() {
    glfwPollEvents();
    glfwGetWindowSize(handle, &width, &height);
    updateFramebuffer();
}

void Window::updateFramebuffer() {
    glfwGetFramebufferSize(handle, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);
}

void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));

    self->windowResize(width, height);
}

void Window::windowResize(int width, int height) {
    this->width = width;
    this->height = height;

    glViewport(0, 0, width, height);
}

// Displays current frame
void Window::present() const {
    glfwSwapBuffers(handle);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(handle);
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