#include "lua_script.hpp"

#include <lua.hpp>

#include <string>
#include <format>

#include "../../nodes/node.hpp"
#include "../scene/scene.hpp"
#include "../util/event_bus.hpp"
#include "../util/util.hpp"
#include "events/lua_event_bindings.hpp"
#include "nodes/lua_node_bindings.hpp"

LuaScript::LuaScript(LuaEngine& engine)
    : engine_(engine) {
}

LuaScript::LuaScript(LuaEngine& engine, Node& node)
    : engine_(engine), attachedNode_(&node) {
}

LuaScript::~LuaScript() {
    releaseEnvironment();
    engine_.removeScript(this);
}

void LuaScript::releaseEnvironment() {
    lua_State* L = engine_.state();
    if (eventBus_)
        eventBus_->unsubscribeOwner(this);
    if (environmentRef_ != LUA_NOREF) {
        luaL_unref(L, LUA_REGISTRYINDEX, environmentRef_);
        environmentRef_ = LUA_NOREF;
    }
}

LoadScriptResult LuaScript::setScript(const char* path, const std::vector<std::string>& flags) {
    lua_State* L = engine_.state();
    releaseEnvironment();
    eventBus_ = attachedNode_ && attachedNode_->getScene()
        ? &attachedNode_->getScene()->eventBus()
        : engine_.eventBus();

    const std::string source = Util::File::read(path);
    if (source.empty()) {
        return {
            .result = LoadResult::Failed,
            .message = std::format("Script '{}' is empty or could not be read", path)
        };
    }

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

    if (luaL_loadbuffer(L, source.c_str(), source.size(), path) != LUA_OK) {
        const char* rawError = lua_tostring(L, -1);
        const std::string error = rawError ? rawError : "Unknown Lua compile error";
        lua_pop(L, 1);
        return {
            .result = LoadResult::Failed,
            .message = error
        };
    }

    lua_newtable(L);
    const int environmentIndex = lua_absindex(L, -1);
    lua_newtable(L);
    lua_pushglobaltable(L);
    lua_setfield(L, -2, "__index");
    lua_setmetatable(L, environmentIndex);
    lua_pushvalue(L, environmentIndex);
    lua_setfield(L, environmentIndex, "_G");

    if (eventBus_)
        LuaEventBindings::pushApi(L, *eventBus_, this);
    else
        lua_pushnil(L);
    lua_setfield(L, environmentIndex, "EventBus");

    if (attachedNode_)
        LuaNodeBindings::pushNodePrototype(L, *attachedNode_);

    if (attachedNode_) {
        lua_setfield(L, environmentIndex, "Node");
    }

    lua_pushvalue(L, environmentIndex);
    environmentRef_ = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_remove(L, environmentIndex);

    lua_rawgeti(L, LUA_REGISTRYINDEX, environmentRef_);
    if (!lua_setupvalue(L, -2, 1)) {
        lua_pop(L, 1);
        releaseEnvironment();
        return {
            .result = LoadResult::Failed,
            .message = "Lua script chunk has no environment upvalue"
        };
    }

    if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
        const char* rawError = lua_tostring(L, -1);
        const std::string error = rawError ? rawError : "Unknown Lua runtime error";
        lua_pop(L, 1);
        releaseEnvironment();
        return {
            .result = LoadResult::Failed,
            .message = error
        };
    }

    return {
        .result = LoadResult::Success,
        .message = {}
    };
}

LoadScriptResult LuaScript::load(const char* path) const {
    const std::string source = Util::File::read(path);

    if (luaL_loadbuffer(this->engine_.state(), source.c_str(), source.size(), path) != LUA_OK || lua_pcall(this->engine_.state(), 0, 0, 0) != LUA_OK) {
        const char* rawError = lua_tostring(this->engine_.state(), -1);
        const std::string error = rawError ? rawError : "Unknown Lua error";
        Util::Log::error(error);
        lua_pop(this->engine_.state(), 1);

        return LoadScriptResult {
            .result = LoadResult::Failed,
            .message = error
        };
    }

    return {};
}

void LuaScript::callLifecycle(const char* name, const float dt, const bool passDelta) const {
    if (environmentRef_ == LUA_NOREF)
        return;

    lua_State* L = engine_.state();
    const int originalTop = lua_gettop(L);
    lua_rawgeti(L, LUA_REGISTRYINDEX, environmentRef_);
    lua_pushstring(L, name);
    lua_rawget(L, -2);
    lua_remove(L, -2);
    if (!lua_isfunction(L, -1)) {
        lua_settop(L, originalTop);
        return;
    }

    if (passDelta)
        lua_pushnumber(L, dt);
    if (lua_pcall(L, passDelta ? 1 : 0, 0, 0) != LUA_OK) {
        const char* error = lua_tostring(L, -1);
        Util::Log::error(error ? error : "Lua lifecycle callback failed");
        lua_pop(L, 1);
    }
    lua_settop(L, originalTop);
}

void LuaScript::ready() const {
    callLifecycle("ready", 0.0f, false);
}

void LuaScript::update(const float dt) const {
    callLifecycle("update", dt, true);
}
