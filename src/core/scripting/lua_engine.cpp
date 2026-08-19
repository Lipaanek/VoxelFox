#include "lua_engine.hpp"

#include <stdexcept>
#include <lua.hpp>
#include <format>
#include <iostream>

#include "lua_script.hpp"
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

lua_State* LuaEngine::state() const {
    return this->L;
}

LoadScriptResult LuaEngine::loadScript(const char* path, const std::vector<std::string>& flags) {
    std::string source = Util::File::read(path);
    this->flags_.clear();
    this->flags_.parse(source);

    for (const auto& flag : flags) {
        if (!this->flags_.has(flag)) {
            return {
                .result = LoadResult::Skipped,
                .message = std::format("Missing required flag: {}, skipping script", flag)
            };
        }
    }

    if (luaL_loadbuffer(this->L, source.c_str(), source.size(), path) != LUA_OK
        || lua_pcall(this->L, 0, 0, 0) != LUA_OK) {
        Util::Log::error(lua_tostring(this->L, -1));
        lua_pop(this->L, 1);

        return {
            .result = LoadResult::Failed,
            .message = lua_tostring(L, -1)
        };
    }

    return {};
}

// bool LuaEngine::hasFlag(const std::string& flag) const {
//     return this->flags_.has(flag);
// }

void LuaEngine::addScript(LuaScript* script) {
    this->scripts_.push_back(script);
}

void LuaEngine::runUpdate(const float dt) const {
    for (const auto& script : this->scripts_) {
        script->update(dt);
    }
}

void LuaEngine::runReady() const {
    for (const auto& script : this->scripts_) {
        script->ready();
    }
}
