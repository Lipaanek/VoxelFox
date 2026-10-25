#include "lua_input_bindings.hpp"

#include <lua.hpp>
#include <stdexcept>

#include "../../input/input_system.hpp"

namespace {

constexpr const char* INPUT_REGISTRY_KEY = "voxelfox_input_system";
constexpr const char* INPUT_SCOPE_REGISTRY_KEY = "voxelfox_input_map_scope";

InputSystem* getInputSystem(lua_State* L) {
    lua_getfield(L, LUA_REGISTRYINDEX, INPUT_REGISTRY_KEY);
    InputSystem* input = static_cast<InputSystem*>(lua_touserdata(L, -1));
    lua_pop(L, 1);

    if (!input)
        luaL_error(L, "Input system is not registered");
    return input;
}

InputMapScope getInputMapScope(lua_State* L) {
    lua_getfield(L, LUA_REGISTRYINDEX, INPUT_SCOPE_REGISTRY_KEY);
    const auto scope = static_cast<InputMapScope>(lua_tointeger(L, -1));
    lua_pop(L, 1);
    return scope;
}

int inputIsActive(lua_State* L) {
    InputSystem* input = getInputSystem(L);
    const char* name = luaL_checkstring(L, 1);
    lua_pushboolean(L, input->isActive(name, getInputMapScope(L)));
    return 1;
}

int inputIsPressed(lua_State* L) {
    InputSystem* input = getInputSystem(L);
    const char* name = luaL_checkstring(L, 1);
    lua_pushboolean(L, input->isPressed(name, getInputMapScope(L)));
    return 1;
}

int inputIsReleased(lua_State* L) {
    InputSystem* input = getInputSystem(L);
    const char* name = luaL_checkstring(L, 1);
    lua_pushboolean(L, input->isReleased(name, getInputMapScope(L)));
    return 1;
}

int inputAxis(lua_State* L) {
    InputSystem* input = getInputSystem(L);
    const char* name = luaL_checkstring(L, 1);
    lua_pushnumber(L, input->getAxis(name, getInputMapScope(L)));
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

void LuaInputBindings::registerInput(
    lua_State* L,
    InputSystem* input,
    const InputMapScope scope
) {
    lua_pushlightuserdata(L, input);
    lua_setfield(L, LUA_REGISTRYINDEX, INPUT_REGISTRY_KEY);

    lua_pushinteger(L, static_cast<lua_Integer>(scope));
    lua_setfield(L, LUA_REGISTRYINDEX, INPUT_SCOPE_REGISTRY_KEY);

    luaL_newlib(L, inputFunctions);
    lua_setglobal(L, "Input");
}

void LuaInputBindings::registerApi(lua_State* L, const LuaApiContext& context) {
    if (!context.inputSystem)
        throw std::invalid_argument("Input API requires an input system service");

    registerInput(L, context.inputSystem, context.inputMapScope);
}
