#include "window.hpp"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <stdexcept>

Window::Window(const char* title, int width, int height) : width(width), height(height) {
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    if (this->width <= 0 || this->height <= 0) {
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        this->width = mode->width;
        this->height = mode->height;
    }

    glfwSetErrorCallback([](int code, const char* desc) {
        fprintf(stderr, "GLFW error %d: %s\n", code, desc);
    });

    this->handle = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!this->handle) {
        glfwTerminate();
        throw std::runtime_error("Failed to initialize window");
    }

    glfwMakeContextCurrent(handle);

    if (!gladLoadGL())
        throw std::runtime_error("Failed to init glad");

    glEnable(GL_DEPTH_TEST);
}

Window::~Window() {
    if (handle)
        glfwDestroyWindow(handle);

    glfwTerminate();
}

void Window::update() {
    glfwPollEvents();
}

// Displays current frame
void Window::present() {
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
    return static_cast<float>(this->width) / this->height;
}