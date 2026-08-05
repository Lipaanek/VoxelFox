#pragma once

#include <string>
#include <unordered_set>

#include "action_map.hpp"

struct GLFWwindow;

class InputSystem {
public:
    explicit InputSystem(GLFWwindow* window);
    ~InputSystem();

    void beginInput();

    void setDefaultBindings();

    void addBinding(const std::string& name, const Binding& binding);
    void clearBindings(const std::string& name);

    bool isActive(const std::string& name) const;
    bool isPressed(const std::string& name) const;
    bool isReleased(const std::string& name) const;
    float getAxis(const std::string& name) const;

    void setCursorCaptured(bool captured);

private:
    GLFWwindow* window;

    ActionMap actionMap;

    std::unordered_set<int> heldKeys;
    std::unordered_set<int> pressedKeys;
    std::unordered_set<int> releasedKeys;
    std::unordered_set<int> heldButtons;
    std::unordered_set<int> pressedButtons;
    std::unordered_set<int> releasedButtons;

    double lastMouseX = 0.0;
    double lastMouseY = 0.0;
    glm::vec2 mouseDelta { 0.0f };
    glm::vec2 scrollDelta { 0.0f };

    RawInputState raw;

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};
