#include "lua_engine.hpp"

#include <stdexcept>

#include <lua.hpp>

#include "../util/util.hpp"

LuaEngine::LuaEngine() {
    this->L = luaL_newstate();
    if (!this->L)
        throw std::runtime_error("Failed to create Lua state");

    luaL_openlibs(this->L);
}

LuaEngine::~LuaEngine() {
    if (this->L)
        lua_close(this->L);
}

lua_State* LuaEngine::state() {
    return this->L;
}

bool LuaEngine::loadScript(const char* path) {
    if (luaL_loadfile(this->L, path) != LUA_OK || lua_pcall(this->L, 0, 0, 0) != LUA_OK) {
        Util::Log::error(lua_tostring(this->L, -1));
        lua_pop(this->L, 1);
        return false;
    }
    return true;
}

void LuaEngine::runUpdate(float dt) {
    lua_getglobal(this->L, "update");
    if (lua_type(this->L, -1) != LUA_TFUNCTION) {
        lua_pop(this->L, 1);
        return;
    }

    lua_pushnumber(this->L, dt);
    if (lua_pcall(this->L, 1, 0, 0) != LUA_OK) {
        Util::Log::error(lua_tostring(this->L, -1));
        lua_pop(this->L, 1);
    }
}

void LuaEngine::runOnReady() {
    lua_getglobal(this->L, "on_ready");
    if (lua_type(this->L, -1) != LUA_TFUNCTION) {
        lua_pop(this->L, 1);
        return;
    }

    if (lua_pcall(this->L, 0, 0, 0) != LUA_OK) {
        Util::Log::error(lua_tostring(this->L, -1));
    }
}
