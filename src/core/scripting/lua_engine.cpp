#include "lua_engine.hpp"

#include <stdexcept>
#include <lua.hpp>
#include <format>

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

LoadScriptResult LuaEngine::loadScript(const char* path, const std::vector<std::string>& flags) {
    std::string source = Util::File::read(path);
    this->flags_.clear();
    this->flags_.parse(source);

    for (const auto& flag : flags) {
        if (!this->flags_.has(flag)) {
            /* Util::Log::error(
                "Script '%s' is missing required flag '%s'",
                path,
                flag.c_str()
            ); */
            return {LoadResult::Skipped, std::format("Missing required flag: {}, skipping script", flag.c_str())};
        }
    }

    if (luaL_loadbuffer(this->L, source.c_str(), source.size(), path) != LUA_OK
        || lua_pcall(this->L, 0, 0, 0) != LUA_OK) {
        Util::Log::error(lua_tostring(this->L, -1));
        lua_pop(this->L, 1);

        return {LoadResult::Failed, lua_tostring(L, -1)};
    }

    return {};
}

bool LuaEngine::hasFlag(const std::string& flag) const {
    return this->flags_.has(flag);
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
