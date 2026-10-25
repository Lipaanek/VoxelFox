#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <glm/glm.hpp>

#include "binding.hpp"

struct RawInputState {
    const std::unordered_set<int>& heldKeys;
    const std::unordered_set<int>& pressedKeys;
    const std::unordered_set<int>& releasedKeys;
    const std::unordered_set<int>& heldButtons;
    const std::unordered_set<int>& pressedButtons;
    const std::unordered_set<int>& releasedButtons;
    const glm::vec2& mouseDelta;
    const glm::vec2& scrollDelta;
};

class ActionMap {
    using BindingMap = std::unordered_map<std::string, std::vector<Binding>>;

private:
    BindingMap editorBindings;
    BindingMap runtimeBindings;

    BindingMap& bindingsFor(InputMapScope scope);
    const BindingMap& bindingsFor(InputMapScope scope) const;
    
public:
    void addBinding(
        const std::string& name,
        const Binding& binding,
        InputMapScope scope = InputMapScope::Runtime
    );
    void clearBindings(const std::string& name, InputMapScope scope = InputMapScope::Runtime);
    void clear();
    void clear(InputMapScope scope);

    bool isActive(
        const std::string& name,
        const RawInputState& raw,
        bool allowKeyboard = true,
        bool allowMouse = true,
        InputMapScope scope = InputMapScope::Runtime
    ) const;
    bool isPressed(
        const std::string& name,
        const RawInputState& raw,
        bool allowKeyboard = true,
        bool allowMouse = true,
        InputMapScope scope = InputMapScope::Runtime
    ) const;
    bool isReleased(
        const std::string& name,
        const RawInputState& raw,
        bool allowKeyboard = true,
        bool allowMouse = true,
        InputMapScope scope = InputMapScope::Runtime
    ) const;
    float getAxis(
        const std::string& name,
        const RawInputState& raw,
        bool allowMouse = true,
        InputMapScope scope = InputMapScope::Runtime
    ) const;
};
