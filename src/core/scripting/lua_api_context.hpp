#pragma once

class Camera;
class InputSystem;
namespace Util {
    class EventBus;
}

struct LuaApiContext {
    Camera* camera = nullptr;
    InputSystem* inputSystem = nullptr;
    Util::EventBus* eventBus = nullptr;
};
