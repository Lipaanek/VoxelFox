#pragma once

class Camera;
class InputSystem;

struct LuaApiContext {
    Camera* camera = nullptr;
    InputSystem* inputSystem = nullptr;
};
