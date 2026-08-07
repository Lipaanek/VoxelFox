#pragma once

struct lua_State;

namespace LuaVector3Bindings {
    void pushVector3(lua_State* L, float x, float y, float z);
    void registerVector3(lua_State* L);
}
