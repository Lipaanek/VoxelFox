#include "action_map.hpp"

ActionMap::BindingMap& ActionMap::bindingsFor(const InputMapScope scope) {
    return scope == InputMapScope::Editor ? this->m_editorBindings : this->m_runtimeBindings;
}

const ActionMap::BindingMap& ActionMap::bindingsFor(const InputMapScope scope) const {
    return scope == InputMapScope::Editor ? this->m_editorBindings : this->m_runtimeBindings;
}

void ActionMap::addBinding(
    const std::string& name,
    const Binding& binding,
    const InputMapScope scope
) {
    this->bindingsFor(scope)[name].push_back(binding);
}

void ActionMap::clearBindings(const std::string& name, const InputMapScope scope) {
    this->bindingsFor(scope).erase(name);
}

void ActionMap::clear() {
    this->m_editorBindings.clear();
    this->m_runtimeBindings.clear();
}

void ActionMap::clear(const InputMapScope scope) {
    this->bindingsFor(scope).clear();
}

bool ActionMap::isActive(
    const std::string& name,
    const RawInputState& raw,
    const bool allowKeyboard,
    const bool allowMouse,
    const InputMapScope scope
) const {
    const auto& bindings = this->bindingsFor(scope);
    auto it = bindings.find(name);
    if (it == bindings.end())
        return false;

    for (const Binding& b : it->second) {
        switch (b.src) {
            case InputSource::Key:
                if (allowKeyboard && raw.heldKeys.contains(b.key))
                    return true;
                break;
            case InputSource::MouseButton:
                if (allowMouse && raw.heldButtons.contains(b.button))
                    return true;
                break;
            case InputSource::MouseAxis:
            case InputSource::ScrollAxis:
                break;
        }
    }
    return false;
}

bool ActionMap::isPressed(
    const std::string& name,
    const RawInputState& raw,
    const bool allowKeyboard,
    const bool allowMouse,
    const InputMapScope scope
) const {
    const auto& bindings = this->bindingsFor(scope);
    auto it = bindings.find(name);
    if (it == bindings.end())
        return false;

    for (const Binding& b : it->second) {
        switch (b.src) {
            case InputSource::Key:
                if (allowKeyboard && raw.pressedKeys.contains(b.key))
                    return true;
                break;
            case InputSource::MouseButton:
                if (allowMouse && raw.pressedButtons.contains(b.button))
                    return true;
                break;
            case InputSource::MouseAxis:
            case InputSource::ScrollAxis:
                break;
        }
    }
    return false;
}

bool ActionMap::isReleased(
    const std::string& name,
    const RawInputState& raw,
    const bool allowKeyboard,
    const bool allowMouse,
    const InputMapScope scope
) const {
    const auto& bindings = this->bindingsFor(scope);
    auto it = bindings.find(name);
    if (it == bindings.end())
        return false;

    for (const Binding& b : it->second) {
        switch (b.src) {
            case InputSource::Key:
                if (allowKeyboard && raw.releasedKeys.contains(b.key))
                    return true;
                break;
            case InputSource::MouseButton:
                if (allowMouse && raw.releasedButtons.contains(b.button))
                    return true;
                break;
            case InputSource::MouseAxis:
            case InputSource::ScrollAxis:
                break;
        }
    }
    return false;
}

float ActionMap::getAxis(
    const std::string& name,
    const RawInputState& raw,
    const bool allowMouse,
    const InputMapScope scope
) const {
    const auto& bindings = this->bindingsFor(scope);
    auto it = bindings.find(name);
    if (it == bindings.end())
        return 0.0f;

    float value = 0.0f;
    for (const Binding& b : it->second) {
        switch (b.src) {
            case InputSource::MouseAxis:
                if (allowMouse)
                    value += (b.axis == 0 ? raw.mouseDelta.x : raw.mouseDelta.y) * b.scale;
                break;
            case InputSource::ScrollAxis:
                if (allowMouse)
                    value += (b.axis == 0 ? raw.scrollDelta.x : raw.scrollDelta.y) * b.scale;
                break;
            case InputSource::Key:
            case InputSource::MouseButton:
                break;
        }
    }
    return value;
}
