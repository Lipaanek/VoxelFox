#include "action_map.hpp"

void ActionMap::addBinding(const std::string& name, const Binding& binding) {
    this->bindings[name].push_back(binding);
}

void ActionMap::clearBindings(const std::string& name) {
    this->bindings.erase(name);
}

void ActionMap::clear() {
    this->bindings.clear();
}

bool ActionMap::isActive(const std::string& name, const RawInputState& raw) const {
    auto it = this->bindings.find(name);
    if (it == this->bindings.end())
        return false;

    for (const Binding& b : it->second) {
        switch (b.src) {
            case InputSource::Key:
                if (raw.heldKeys.contains(b.key))
                    return true;
                break;
            case InputSource::MouseButton:
                if (raw.heldButtons.contains(b.button))
                    return true;
                break;
            case InputSource::MouseAxis:
            case InputSource::ScrollAxis:
                break;
        }
    }
    return false;
}

bool ActionMap::isPressed(const std::string& name, const RawInputState& raw) const {
    auto it = this->bindings.find(name);
    if (it == this->bindings.end())
        return false;

    for (const Binding& b : it->second) {
        switch (b.src) {
            case InputSource::Key:
                if (raw.pressedKeys.contains(b.key))
                    return true;
                break;
            case InputSource::MouseButton:
                if (raw.pressedButtons.contains(b.button))
                    return true;
                break;
            case InputSource::MouseAxis:
            case InputSource::ScrollAxis:
                break;
        }
    }
    return false;
}

bool ActionMap::isReleased(const std::string& name, const RawInputState& raw) const {
    auto it = this->bindings.find(name);
    if (it == this->bindings.end())
        return false;

    for (const Binding& b : it->second) {
        switch (b.src) {
            case InputSource::Key:
                if (raw.releasedKeys.contains(b.key))
                    return true;
                break;
            case InputSource::MouseButton:
                if (raw.releasedButtons.contains(b.button))
                    return true;
                break;
            case InputSource::MouseAxis:
            case InputSource::ScrollAxis:
                break;
        }
    }
    return false;
}

float ActionMap::getAxis(const std::string& name, const RawInputState& raw) const {
    auto it = this->bindings.find(name);
    if (it == this->bindings.end())
        return 0.0f;

    float value = 0.0f;
    for (const Binding& b : it->second) {
        switch (b.src) {
            case InputSource::MouseAxis:
                value += (b.axis == 0 ? raw.mouseDelta.x : raw.mouseDelta.y) * b.scale;
                break;
            case InputSource::ScrollAxis:
                value += (b.axis == 0 ? raw.scrollDelta.x : raw.scrollDelta.y) * b.scale;
                break;
            case InputSource::Key:
            case InputSource::MouseButton:
                break;
        }
    }
    return value;
}
