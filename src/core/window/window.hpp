#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window {
private:
    GLFWwindow* handle = nullptr;
    int width;
    int height;

public:
    Window(const char* title, int width, int height);
    ~Window();

    bool shouldClose() const;
    void update();
    void present();

    GLFWwindow* getHandle() const;
    int getWidth() const;
    int getHeight() const;
    float getAspect() const;
};