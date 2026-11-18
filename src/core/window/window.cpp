#include "window.hpp"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <stdexcept>

Window::Window(const WindowSettings &settings)
    : m_callbackContext {.window = this, .inputSystem = nullptr }, m_width(settings.width), m_height(settings.height) {
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    if (this->m_width <= 0 || this->m_height <= 0) {
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        this->m_width = mode->width;
        this->m_height = mode->height;
    }

    glfwSetErrorCallback([](const int code, const char* desc) {
        fprintf(stderr, "GLFW error %d: %s\n", code, desc);
    });

    this->m_handle = glfwCreateWindow(m_width, m_height, settings.title, nullptr, nullptr);

    if (!this->m_handle) {
        glfwTerminate();
        throw std::runtime_error("Failed to initialize window");
    }

    glfwMakeContextCurrent(this->m_handle);

    glfwWindowHint(GLFW_MAXIMIZED, static_cast<int>(settings.type));
    glfwSetWindowAttrib(this->m_handle, GLFW_MAXIMIZED, GLFW_TRUE);
    glfwMaximizeWindow(this->m_handle);

    if (!gladLoadGL())
        throw std::runtime_error("Failed to init glad");

    glfwGetWindowSize(this->m_handle, &m_width, &m_height);
    updateFramebuffer();

    glEnable(GL_DEPTH_TEST);

    // V-Sync
    glfwSwapInterval(static_cast<int>(settings.vsync));

    // Window resize handling
    glfwSetWindowUserPointer(this->m_handle, &m_callbackContext);
    glfwSetFramebufferSizeCallback(this->m_handle, Window::framebufferSizeCallback);
}

Window::~Window() {
    if (this->m_handle) {
        glfwSetWindowUserPointer(this->m_handle, nullptr);
        glfwDestroyWindow(this->m_handle);
        this->m_handle = nullptr;
    }

    glfwTerminate();
}

void Window::update() {
    glfwPollEvents();
    glfwGetWindowSize(this->m_handle, &m_width, &m_height);
    updateFramebuffer();
}

void Window::updateFramebuffer() {
    glfwGetFramebufferSize(this->m_handle, &m_framebufferWidth, &m_framebufferHeight);
    glViewport(0, 0, m_framebufferWidth, m_framebufferHeight);
}

void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    const auto* context = static_cast<WindowCallbackContext*>(glfwGetWindowUserPointer(window));
    if (!context || !context->window)
        return;

    context->window->windowResize(width, height);
}

void Window::windowResize(int width, int height) {
    this->m_width = width;
    this->m_height = height;

    glViewport(0, 0, width, height);
}

// Displays current frame
void Window::present() const {
    glfwSwapBuffers(this->m_handle);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(this->m_handle);
}

GLFWwindow* Window::getHandle() const {
    return this->m_handle;
}

int Window::getWidth() const {
    return this->m_width;
}

int Window::getHeight() const {
    return this->m_height;
}

float Window::getAspect() const {
    if (m_framebufferWidth <= 0 || m_framebufferHeight <= 0)
        return 1.0f;

    return static_cast<float>(m_framebufferWidth) / static_cast<float>(m_framebufferHeight);
}