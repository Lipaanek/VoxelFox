#include "lua_vector3.hpp"

#include <lua.hpp>

namespace {

int vector3New(lua_State* L) {
    LuaVector3Bindings::pushVector3(L,
        static_cast<float>(luaL_optnumber(L, 1, 0.0)),
        static_cast<float>(luaL_optnumber(L, 2, 0.0)),
        static_cast<float>(luaL_optnumber(L, 3, 0.0)));
    return 1;
}

const luaL_Reg vector3Functions[] = {
    { "new", vector3New },
    { nullptr, nullptr }
};

} // namespace

void LuaVector3Bindings::pushVector3(lua_State* L, float x, float y, float z) {
    lua_createtable(L, 0, 3);
    lua_pushnumber(L, x);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, y);
    lua_setfield(L, -2, "y");
    lua_pushnumber(L, z);
    lua_setfield(L, -2, "z");
}

void LuaVector3Bindings::registerVector3(lua_State* L) {
    luaL_newlib(L, vector3Functions);
    lua_setglobal(L, "Vector3");
}
