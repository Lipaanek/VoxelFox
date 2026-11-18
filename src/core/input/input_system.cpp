#include "input_system.hpp"

#include <stdexcept>

#include "core/window/window.hpp"
#include <GLFW/glfw3.h>

InputSystem::InputSystem(Window& window)
    : m_window(window.getHandle()),
      m_callbackContext(&window.m_callbackContext),
      m_rawInputState { this->m_heldKeys,
            this->m_pressedKeys,
            this->m_releasedKeys,
            this->m_heldButtons,
            this->m_pressedButtons,
            this->m_releasedButtons,
            this->m_mouseDelta,
            this->m_scrollDelta } {
    if (!this->m_window)
        throw std::invalid_argument("InputSystem requires a valid window");
    if (this->m_callbackContext->inputSystem)
        throw std::logic_error("Window already has an input system");

    this->m_callbackContext->inputSystem = this;
    glfwSetKeyCallback(this->m_window, keyCallback);
    glfwSetMouseButtonCallback(this->m_window, mouseButtonCallback);
    glfwSetCursorPosCallback(this->m_window, cursorPosCallback);
    glfwSetScrollCallback(this->m_window, scrollCallback);

    glfwGetCursorPos(this->m_window, &this->m_lastMouseX, &this->m_lastMouseY);
}

InputSystem::~InputSystem() {
    if (!this->m_window)
        return;

    if (this->m_callbackContext && this->m_callbackContext->inputSystem == this)
        this->m_callbackContext->inputSystem = nullptr;

    glfwSetKeyCallback(this->m_window, nullptr);
    glfwSetMouseButtonCallback(this->m_window, nullptr);
    glfwSetCursorPosCallback(this->m_window, nullptr);
    glfwSetScrollCallback(this->m_window, nullptr);
}

void InputSystem::beginInput() {
    this->m_pressedKeys.clear();
    this->m_releasedKeys.clear();
    this->m_pressedButtons.clear();
    this->m_releasedButtons.clear();
    this->m_mouseDelta = { 0.0f, 0.0f };
    this->m_scrollDelta = { 0.0f, 0.0f };
}

void InputSystem::setEditorDefaultBindings() {
    constexpr InputMapScope scope = InputMapScope::Editor;
    this->addBinding("move_forward", { ActionType::Bool, InputSource::Key, GLFW_KEY_W }, scope);
    this->addBinding("move_forward", { ActionType::Bool, InputSource::Key, GLFW_KEY_UP }, scope);
    this->addBinding("move_back", { ActionType::Bool, InputSource::Key, GLFW_KEY_S }, scope);
    this->addBinding("move_back", { ActionType::Bool, InputSource::Key, GLFW_KEY_DOWN }, scope);
    this->addBinding("move_left", { ActionType::Bool, InputSource::Key, GLFW_KEY_A }, scope);
    this->addBinding("move_left", { ActionType::Bool, InputSource::Key, GLFW_KEY_LEFT }, scope);
    this->addBinding("move_right", { ActionType::Bool, InputSource::Key, GLFW_KEY_D }, scope);
    this->addBinding("move_right", { ActionType::Bool, InputSource::Key, GLFW_KEY_RIGHT }, scope);
    this->addBinding("move_up", { ActionType::Bool, InputSource::Key, GLFW_KEY_SPACE }, scope);
    this->addBinding("move_down", { ActionType::Bool, InputSource::Key, GLFW_KEY_LEFT_SHIFT }, scope);
    this->addBinding("boost", { ActionType::Bool, InputSource::Key, GLFW_KEY_LEFT_CONTROL }, scope);

    this->addBinding(
        "rotate_focus",
        { ActionType::Bool, InputSource::MouseButton, 0, GLFW_MOUSE_BUTTON_RIGHT },
        scope
    );

    this->addBinding("look_x", { ActionType::Axis, InputSource::MouseAxis, 0, 0, 0 }, scope);
    this->addBinding("look_y", { ActionType::Axis, InputSource::MouseAxis, 0, 0, 1 }, scope);
    this->addBinding("zoom", { ActionType::Axis, InputSource::ScrollAxis, 0, 0, 1 }, scope);
}

void InputSystem::addBinding(
    const std::string& name,
    const Binding& binding,
    const InputMapScope scope
) {
    this->m_actionMap.addBinding(name, binding, scope);
}

void InputSystem::clearBindings(const std::string& name, const InputMapScope scope) {
    this->m_actionMap.clearBindings(name, scope);
}

void InputSystem::clearBindings(const InputMapScope scope) {
    this->m_actionMap.clear(scope);
}

bool InputSystem::isActive(const std::string& name, const InputMapScope scope) const {
    return this->m_actionMap.isActive(
        name,
        this->m_rawInputState,
        !this->m_uiCapturesKeyboard,
        !this->m_uiCapturesMouse,
        scope
    );
}

bool InputSystem::isPressed(const std::string& name, const InputMapScope scope) const {
    return this->m_actionMap.isPressed(
        name,
        this->m_rawInputState,
        !this->m_uiCapturesKeyboard,
        !this->m_uiCapturesMouse,
        scope
    );
}

bool InputSystem::isReleased(const std::string& name, const InputMapScope scope) const {
    return this->m_actionMap.isReleased(
        name,
        this->m_rawInputState,
        !this->m_uiCapturesKeyboard,
        !this->m_uiCapturesMouse,
        scope
    );
}

float InputSystem::getAxis(const std::string& name, const InputMapScope scope) const {
    return this->m_actionMap.getAxis(name, this->m_rawInputState, !this->m_uiCapturesMouse, scope);
}

void InputSystem::setUICapture(const bool mouse, const bool keyboard) {
    this->m_uiCapturesMouse = mouse;
    this->m_uiCapturesKeyboard = keyboard;
}

void InputSystem::setCursorCaptured(bool captured) {
    glfwSetInputMode(this->m_window, GLFW_CURSOR,
        captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void InputSystem::keyCallback(GLFWwindow* window, int key, int, int action, int) {
    auto* context = static_cast<WindowCallbackContext*>(glfwGetWindowUserPointer(window));
    InputSystem* self = context ? context->inputSystem : nullptr;
    if (!self)
        return;

    if (action == GLFW_PRESS) {
        self->m_heldKeys.insert(key);
        self->m_pressedKeys.insert(key);
    } else if (action == GLFW_RELEASE) {
        self->m_heldKeys.erase(key);
        self->m_releasedKeys.insert(key);
    }
}

void InputSystem::mouseButtonCallback(GLFWwindow* window, int button, int action, int) {
    auto* context = static_cast<WindowCallbackContext*>(glfwGetWindowUserPointer(window));
    InputSystem* self = context ? context->inputSystem : nullptr;
    if (!self)
        return;

    if (action == GLFW_PRESS) {
        self->m_heldButtons.insert(button);
        self->m_pressedButtons.insert(button);
    } else if (action == GLFW_RELEASE) {
        self->m_heldButtons.erase(button);
        self->m_releasedButtons.insert(button);
    }
}

void InputSystem::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    auto* context = static_cast<WindowCallbackContext*>(glfwGetWindowUserPointer(window));
    InputSystem* self = context ? context->inputSystem : nullptr;
    if (!self)
        return;

    self->m_mouseDelta.x += static_cast<float>(xpos - self->m_lastMouseX);
    self->m_mouseDelta.y += static_cast<float>(ypos - self->m_lastMouseY);
    self->m_lastMouseX = xpos;
    self->m_lastMouseY = ypos;
}

void InputSystem::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    auto* context = static_cast<WindowCallbackContext*>(glfwGetWindowUserPointer(window));
    InputSystem* self = context ? context->inputSystem : nullptr;
    if (!self)
        return;

    self->m_scrollDelta.x += static_cast<float>(xoffset);
    self->m_scrollDelta.y += static_cast<float>(yoffset);
}
