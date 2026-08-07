#include "lua_input_bindings.hpp"

#include <lua.hpp>

#include "../../input/input_system.hpp"

namespace {

constexpr const char* INPUT_REGISTRY_KEY = "voxelfox_input_system";

InputSystem* getInputSystem(lua_State* L) {
    lua_getfield(L, LUA_REGISTRYINDEX, INPUT_REGISTRY_KEY);
    InputSystem* input = static_cast<InputSystem*>(lua_touserdata(L, -1));
    lua_pop(L, 1);

    if (!input)
        luaL_error(L, "Input system is not registered");
    return input;
}

int inputIsActive(lua_State* L) {
    InputSystem* input = getInputSystem(L);
    const char* name = luaL_checkstring(L, 1);
    lua_pushboolean(L, input->isActive(name));
    return 1;
}

int inputIsPressed(lua_State* L) {
    InputSystem* input = getInputSystem(L);
    const char* name = luaL_checkstring(L, 1);
    lua_pushboolean(L, input->isPressed(name));
    return 1;
}

int inputIsReleased(lua_State* L) {
    InputSystem* input = getInputSystem(L);
    const char* name = luaL_checkstring(L, 1);
    lua_pushboolean(L, input->isReleased(name));
    return 1;
}

int inputAxis(lua_State* L) {
    InputSystem* input = getInputSystem(L);
    const char* name = luaL_checkstring(L, 1);
    lua_pushnumber(L, input->getAxis(name));
    return 1;
}

int inputCaptureMouse(lua_State* L) {
    InputSystem* input = getInputSystem(L);
    input->setCursorCaptured(lua_toboolean(L, 1));
    return 0;
}

const luaL_Reg inputFunctions[] = {
    { "is_active", inputIsActive },
    { "is_pressed", inputIsPressed },
    { "is_released", inputIsReleased },
    { "axis", inputAxis },
    { "capture_mouse", inputCaptureMouse },
    { nullptr, nullptr }
};

} // namespace

void LuaInputBindings::registerInput(lua_State* L, InputSystem* input) {
    lua_pushlightuserdata(L, input);
    lua_setfield(L, LUA_REGISTRYINDEX, INPUT_REGISTRY_KEY);

    luaL_newlib(L, inputFunctions);
    lua_setglobal(L, "Input");
}
