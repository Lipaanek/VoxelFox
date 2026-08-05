#pragma once

struct lua_State;
class InputSystem;

namespace LuaInputBindings {
    void registerInput(lua_State* L, InputSystem* input);
}
