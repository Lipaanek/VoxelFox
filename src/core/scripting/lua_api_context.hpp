#pragma once

#include "core/input/binding.hpp"

class Camera;
class InputSystem;
namespace Util {
    class EventBus;
}

struct LuaApiContext {
    Camera* camera = nullptr;
    InputSystem* inputSystem = nullptr;
    InputMapScope inputMapScope = InputMapScope::Runtime;
    Util::EventBus* eventBus = nullptr;
};
