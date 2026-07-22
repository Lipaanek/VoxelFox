#include "window.hpp"
#include <GLFW/glfw3.h>
#include <stdexcept>

Window::Window(const char* title, int width, int height) {
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    this->handle = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!this->handle) {
        glfwTerminate();
        throw std::runtime_error("Failed to initialize window");
    }

    glfwMakeContextCurrent(handle);
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