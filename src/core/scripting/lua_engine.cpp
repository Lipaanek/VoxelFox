#include "lua_engine.hpp"

#include <stdexcept>
#include <lua.hpp>
#include <format>
#include <iostream>
#include <algorithm>

#include "lua_script.hpp"
#include "../util/event_bus.hpp"
#include "../util/util.hpp"

static int luaPrint(lua_State* L) {
    const int n = lua_gettop(L);
    std::string output;
    for (int i = 1; i <= n; i++) {
        if (i > 1) output += '\t';
        size_t len;
        if (const char* s = lua_tolstring(L, i, &len)) {
            output.append(s, len);
        } else {
            output += "(nil)";
        }
    }
    Util::Log::log(output);
    return 0;
}

LuaEngine::LuaEngine() {
    this->m_L = luaL_newstate();
    if (!this->m_L)
        throw std::runtime_error("Failed to create Lua state");

    luaL_openlibs(this->m_L);

    lua_pushcfunction(this->m_L, luaPrint);
    lua_setglobal(this->m_L, "print");
}

LuaEngine::~LuaEngine() {
    if (this->m_eventBus_)
        this->m_eventBus_->unsubscribeOwner(this->m_L);
    if (this->m_L)
        lua_close(this->m_L);
}

lua_State* LuaEngine::state() const {
    return this->m_L;
}

Util::EventBus* LuaEngine::eventBus() const {
    return this->m_eventBus_;
}

void LuaEngine::registerApiBindings(const LuaApiRegistry& registry, const LuaApiContext& context) {
    if (m_apiBindingsRegistered_)
        throw std::logic_error("Lua API bindings are already registered");

    this->m_eventBus_ = context.eventBus;
    registry.registerAll(this->m_L, context);
    m_apiBindingsRegistered_ = true;
}

LoadScriptResult LuaEngine::loadScript(const char* path, const std::vector<std::string>& flags) {
    const std::string source = Util::File::read(path);
    this->m_flags_.clear();
    this->m_flags_.parse(source);

    for (const auto& flag : flags) {
        if (!this->m_flags_.has(flag)) {
            return {
                .result = LoadResult::Skipped,
                .message = std::format("Missing required flag: {}, skipping script", flag)
            };
        }
    }

    if (luaL_loadbuffer(this->m_L, source.c_str(), source.size(), path) != LUA_OK
        || lua_pcall(this->m_L, 0, 0, 0) != LUA_OK) {
        const char* rawError = lua_tostring(this->m_L, -1);
        const std::string error = rawError ? rawError : "Unknown Lua load error";
        Util::Log::error(error);
        lua_pop(this->m_L, 1);

        return {
            .result = LoadResult::Failed,
            .message = error
        };
    }

    return {};
}

void LuaEngine::addScript(LuaScript* script) {
    if (script && std::find(this->m_scripts_.begin(), this->m_scripts_.end(), script) == this->m_scripts_.end())
        this->m_scripts_.push_back(script);
}

void LuaEngine::removeScript(const LuaScript* script) {
    std::erase(this->m_scripts_, script);
}

void LuaEngine::runUpdate(const float dt) const {
    const std::vector<LuaScript*> scripts = this->m_scripts_;
    for (const LuaScript* script : scripts) {
        if (std::find(this->m_scripts_.begin(), this->m_scripts_.end(), script) != this->m_scripts_.end())
            script->update(dt);
    }
}

void LuaEngine::runReady() const {
    Util::Log::log("Called ready");
    const std::vector<LuaScript*> scripts = this->m_scripts_;
    for (const LuaScript* script : scripts) {
        if (std::find(this->m_scripts_.begin(), this->m_scripts_.end(), script) != this->m_scripts_.end())
            script->ready();
    }
}
