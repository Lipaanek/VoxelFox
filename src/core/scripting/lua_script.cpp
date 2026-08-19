#include "lua_script.hpp"
#include <format>
#include "../util/util.hpp"

LuaScript::LuaScript(LuaEngine &engine) : engine_(engine) {}

LoadScriptResult LuaScript::setScript(const char* path, const std::vector<std::string>& flags) {
    lua_State* L = engine_.state();

    if (instanceRef_ != LUA_NOREF) {
        luaL_unref(L, LUA_REGISTRYINDEX, instanceRef_);
        instanceRef_ = LUA_NOREF;
    }

    const std::string source = Util::File::read(path);

    LuaFlags scriptFlags;
    scriptFlags.parse(source);

    for (const auto& flag : flags) {
        if (!scriptFlags.has(flag)) {
            return {
                .result = LoadResult::Skipped,
                .message = std::format(
                    "Missing required flag: {}, skipping script",
                    flag
                )
            };
        }
    }

    if (luaL_loadbuffer(
            L,
            source.c_str(),
            source.size(),
            path
        ) != LUA_OK)
    {

        const std::string error = lua_tostring(L, -1);
        lua_pop(L, 1);

        return {
            .result = LoadResult::Failed,
            .message = error
        };
        }

    if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
        std::string error = lua_tostring(L, -1);
        lua_pop(L, 1);

        return {
            .result = LoadResult::Failed,
            .message = error
        };
    }

    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);

        return {
            .result = LoadResult::Failed,
            .message = std::format(
                "Script '{}' must return a table",
                path
            )
        };
    }

    instanceRef_ = luaL_ref(L, LUA_REGISTRYINDEX);

    return {};
}

LoadScriptResult LuaScript::load(const char* path) const {
    const std::string source = Util::File::read(path);

    if (luaL_loadbuffer(this->engine_.state(), source.c_str(), source.size(), path) != LUA_OK || lua_pcall(this->engine_.state(), 0, 0, 0) != LUA_OK) {
        Util::Log::error(lua_tostring(this->engine_.state(), -1));
        lua_pop(this->engine_.state(), 1);

        return LoadScriptResult {
            .result = LoadResult::Failed,
            .message = lua_tostring(this->engine_.state(), -1)
        };
    }

    return {};
}

void LuaScript::ready() const {
    this->engine_.runReady();
}

void LuaScript::update(const float dt) const {
    this->engine_.runUpdate(dt);
}
