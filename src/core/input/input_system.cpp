#include "input_system.hpp"

#include <GLFW/glfw3.h>

InputSystem::InputSystem(GLFWwindow* window)
    : window(window),
      raw { this->heldKeys,
            this->pressedKeys,
            this->releasedKeys,
            this->heldButtons,
            this->pressedButtons,
            this->releasedButtons,
            this->mouseDelta,
            this->scrollDelta } {
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);

    glfwGetCursorPos(window, &this->lastMouseX, &this->lastMouseY);
}

InputSystem::~InputSystem() {
    if (!this->window)
        return;

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

void InputSystem::setDefaultBindings() {
    this->addBinding("move_forward", { ActionType::Bool, InputSource::Key, GLFW_KEY_W });
    this->addBinding("move_forward", { ActionType::Bool, InputSource::Key, GLFW_KEY_UP });
    this->addBinding("move_back", { ActionType::Bool, InputSource::Key, GLFW_KEY_S });
    this->addBinding("move_back", { ActionType::Bool, InputSource::Key, GLFW_KEY_DOWN });
    this->addBinding("move_left", { ActionType::Bool, InputSource::Key, GLFW_KEY_A });
    this->addBinding("move_left", { ActionType::Bool, InputSource::Key, GLFW_KEY_LEFT });
    this->addBinding("move_right", { ActionType::Bool, InputSource::Key, GLFW_KEY_D });
    this->addBinding("move_right", { ActionType::Bool, InputSource::Key, GLFW_KEY_RIGHT });
    this->addBinding("move_up", { ActionType::Bool, InputSource::Key, GLFW_KEY_SPACE });
    this->addBinding("move_down", { ActionType::Bool, InputSource::Key, GLFW_KEY_LEFT_SHIFT });
    this->addBinding("boost", { ActionType::Bool, InputSource::Key, GLFW_KEY_LEFT_CONTROL });

    this->addBinding("look_x", { ActionType::Axis, InputSource::MouseAxis, 0, 0, 0 });
    this->addBinding("look_y", { ActionType::Axis, InputSource::MouseAxis, 0, 0, 1 });
    this->addBinding("zoom", { ActionType::Axis, InputSource::ScrollAxis, 0, 0, 1 });
}

void InputSystem::addBinding(const std::string& name, const Binding& binding) {
    this->actionMap.addBinding(name, binding);
}

void InputSystem::clearBindings(const std::string& name) {
    this->actionMap.clearBindings(name);
}

bool InputSystem::isActive(const std::string& name) const {
    return this->actionMap.isActive(name, this->raw);
}

bool InputSystem::isPressed(const std::string& name) const {
    return this->actionMap.isPressed(name, this->raw);
}

bool InputSystem::isReleased(const std::string& name) const {
    return this->actionMap.isReleased(name, this->raw);
}

float InputSystem::getAxis(const std::string& name) const {
    return this->actionMap.getAxis(name, this->raw);
}

void InputSystem::setCursorCaptured(bool captured) {
    glfwSetInputMode(this->window, GLFW_CURSOR,
        captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void InputSystem::keyCallback(GLFWwindow* window, int key, int, int action, int) {
    InputSystem* self = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
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

void InputSystem::mouseButtonCallback(GLFWwindow* window, int button, int, int action) {
    InputSystem* self = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
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
    InputSystem* self = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
    if (!self)
        return;

    self->mouseDelta.x += static_cast<float>(xpos - self->lastMouseX);
    self->mouseDelta.y += static_cast<float>(ypos - self->lastMouseY);
    self->lastMouseX = xpos;
    self->lastMouseY = ypos;
}

void InputSystem::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    InputSystem* self = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
    if (!self)
        return;

    self->scrollDelta.x += static_cast<float>(xoffset);
    self->scrollDelta.y += static_cast<float>(yoffset);
}
