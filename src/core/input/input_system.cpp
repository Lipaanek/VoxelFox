#include "input_system.hpp"

#include <stdexcept>

#include "core/window/window.hpp"
#include <GLFW/glfw3.h>

InputSystem::InputSystem(Window& window)
    : window(window.getHandle()),
      callbackContext(&window.callbackContext),
      raw { this->heldKeys,
            this->pressedKeys,
            this->releasedKeys,
            this->heldButtons,
            this->pressedButtons,
            this->releasedButtons,
            this->mouseDelta,
            this->scrollDelta } {
    if (!this->window)
        throw std::invalid_argument("InputSystem requires a valid window");
    if (this->callbackContext->inputSystem)
        throw std::logic_error("Window already has an input system");

    this->callbackContext->inputSystem = this;
    glfwSetKeyCallback(this->window, keyCallback);
    glfwSetMouseButtonCallback(this->window, mouseButtonCallback);
    glfwSetCursorPosCallback(this->window, cursorPosCallback);
    glfwSetScrollCallback(this->window, scrollCallback);

    glfwGetCursorPos(this->window, &this->lastMouseX, &this->lastMouseY);
}

InputSystem::~InputSystem() {
    if (!this->window)
        return;

    if (this->callbackContext && this->callbackContext->inputSystem == this)
        this->callbackContext->inputSystem = nullptr;

    glfwSetKeyCallback(this->window, nullptr);
    glfwSetMouseButtonCallback(this->window, nullptr);
    glfwSetCursorPosCallback(this->window, nullptr);
    glfwSetScrollCallback(this->window, nullptr);
}

void InputSystem::beginInput() {
    this->pressedKeys.clear();
    this->releasedKeys.clear();
    this->pressedButtons.clear();
    this->releasedButtons.clear();
    this->mouseDelta = { 0.0f, 0.0f };
    this->scrollDelta = { 0.0f, 0.0f };
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
    this->actionMap.addBinding(name, binding, scope);
}

void InputSystem::clearBindings(const std::string& name, const InputMapScope scope) {
    this->actionMap.clearBindings(name, scope);
}

void InputSystem::clearBindings(const InputMapScope scope) {
    this->actionMap.clear(scope);
}

bool InputSystem::isActive(const std::string& name, const InputMapScope scope) const {
    return this->actionMap.isActive(
        name,
        this->raw,
        !this->uiCapturesKeyboard,
        !this->uiCapturesMouse,
        scope
    );
}

bool InputSystem::isPressed(const std::string& name, const InputMapScope scope) const {
    return this->actionMap.isPressed(
        name,
        this->raw,
        !this->uiCapturesKeyboard,
        !this->uiCapturesMouse,
        scope
    );
}

bool InputSystem::isReleased(const std::string& name, const InputMapScope scope) const {
    return this->actionMap.isReleased(
        name,
        this->raw,
        !this->uiCapturesKeyboard,
        !this->uiCapturesMouse,
        scope
    );
}

float InputSystem::getAxis(const std::string& name, const InputMapScope scope) const {
    return this->actionMap.getAxis(name, this->raw, !this->uiCapturesMouse, scope);
}

void InputSystem::setUICapture(const bool mouse, const bool keyboard) {
    this->uiCapturesMouse = mouse;
    this->uiCapturesKeyboard = keyboard;
}

void InputSystem::setCursorCaptured(bool captured) {
    glfwSetInputMode(this->window, GLFW_CURSOR,
        captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void InputSystem::keyCallback(GLFWwindow* window, int key, int, int action, int) {
    auto* context = static_cast<WindowCallbackContext*>(glfwGetWindowUserPointer(window));
    InputSystem* self = context ? context->inputSystem : nullptr;
    if (!self)
        return;

    if (action == GLFW_PRESS) {
        self->heldKeys.insert(key);
        self->pressedKeys.insert(key);
    } else if (action == GLFW_RELEASE) {
        self->heldKeys.erase(key);
        self->releasedKeys.insert(key);
    }
}

void InputSystem::mouseButtonCallback(GLFWwindow* window, int button, int action, int) {
    auto* context = static_cast<WindowCallbackContext*>(glfwGetWindowUserPointer(window));
    InputSystem* self = context ? context->inputSystem : nullptr;
    if (!self)
        return;

    if (action == GLFW_PRESS) {
        self->heldButtons.insert(button);
        self->pressedButtons.insert(button);
    } else if (action == GLFW_RELEASE) {
        self->heldButtons.erase(button);
        self->releasedButtons.insert(button);
    }
}

void InputSystem::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    auto* context = static_cast<WindowCallbackContext*>(glfwGetWindowUserPointer(window));
    InputSystem* self = context ? context->inputSystem : nullptr;
    if (!self)
        return;

    self->mouseDelta.x += static_cast<float>(xpos - self->lastMouseX);
    self->mouseDelta.y += static_cast<float>(ypos - self->lastMouseY);
    self->lastMouseX = xpos;
    self->lastMouseY = ypos;
}

void InputSystem::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    auto* context = static_cast<WindowCallbackContext*>(glfwGetWindowUserPointer(window));
    InputSystem* self = context ? context->inputSystem : nullptr;
    if (!self)
        return;

    self->scrollDelta.x += static_cast<float>(xoffset);
    self->scrollDelta.y += static_cast<float>(yoffset);
}
