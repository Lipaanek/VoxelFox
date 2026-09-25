#pragma once

#include "../lua_api_context.hpp"

struct lua_State;
class InputSystem;

namespace LuaInputBindings {
    void registerInput(lua_State* L, InputSystem* input);
    void registerApi(lua_State* L, const LuaApiContext& context);
}
