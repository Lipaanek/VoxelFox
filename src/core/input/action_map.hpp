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
private:
    std::unordered_map<std::string, std::vector<Binding>> bindings;
    
public:
    void addBinding(const std::string& name, const Binding& binding);
    void clearBindings(const std::string& name);
    void clear();

    bool isActive(const std::string& name, const RawInputState& raw) const;
    bool isPressed(const std::string& name, const RawInputState& raw) const;
    bool isReleased(const std::string& name, const RawInputState& raw) const;
    float getAxis(const std::string& name, const RawInputState& raw) const;
};
