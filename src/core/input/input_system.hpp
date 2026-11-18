#pragma once

#include <string>
#include <unordered_set>

#include "action_map.hpp"

struct GLFWwindow;
class Window;
struct WindowCallbackContext;

class InputSystem {
private:
    GLFWwindow* m_window;
    WindowCallbackContext* m_callbackContext;

    ActionMap m_actionMap;
    bool m_uiCapturesMouse = false;
    bool m_uiCapturesKeyboard = false;

    std::unordered_set<int> m_heldKeys;
    std::unordered_set<int> m_pressedKeys;
    std::unordered_set<int> m_releasedKeys;
    std::unordered_set<int> m_heldButtons;
    std::unordered_set<int> m_pressedButtons;
    std::unordered_set<int> m_releasedButtons;

    double m_lastMouseX = 0.0;
    double m_lastMouseY = 0.0;
    glm::vec2 m_mouseDelta { 0.0f };
    glm::vec2 m_scrollDelta { 0.0f };

    RawInputState m_rawInputState;

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

public:
    explicit InputSystem(Window& window);
    ~InputSystem();

    InputSystem(const InputSystem&) = delete;
    InputSystem& operator=(const InputSystem&) = delete;
    InputSystem(InputSystem&&) = delete;
    InputSystem& operator=(InputSystem&&) = delete;

    void beginInput();

    void setEditorDefaultBindings();

    void addBinding(
        const std::string& name,
        const Binding& binding,
        InputMapScope scope = InputMapScope::Runtime
    );
    void clearBindings(const std::string& name, InputMapScope scope = InputMapScope::Runtime);
    void clearBindings(InputMapScope scope);

    bool isActive(const std::string& name, InputMapScope scope = InputMapScope::Runtime) const;
    bool isPressed(const std::string& name, InputMapScope scope = InputMapScope::Runtime) const;
    bool isReleased(const std::string& name, InputMapScope scope = InputMapScope::Runtime) const;
    float getAxis(const std::string& name, InputMapScope scope = InputMapScope::Runtime) const;

    void setUICapture(bool mouse, bool keyboard);
    void setCursorCaptured(bool captured);
};
