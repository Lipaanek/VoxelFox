#pragma once

#include <string>
#include <vector>

#include "lua_api_context.hpp"
#include "lua_api_registry.hpp"
#include "lua_flags.hpp"

struct lua_State;

class LuaScript;

enum class LoadResult {
    Success,
    Skipped,
    Failed
};

struct LoadScriptResult {
    LoadResult result;
    std::string message;
};

class LuaEngine {
private:
    lua_State* L;
    LuaFlags flags_;
    std::vector<LuaScript*> scripts_;
    bool apiBindingsRegistered_ = false;

public:
    LuaEngine();
    ~LuaEngine();

    LuaEngine(const LuaEngine&) = delete;
    LuaEngine& operator=(const LuaEngine&) = delete;

    LuaEngine(LuaEngine&&) = delete;
    LuaEngine& operator=(LuaEngine&&) = delete;

    [[nodiscard]] lua_State* state() const;
    void registerApiBindings(const LuaApiRegistry& registry, const LuaApiContext& context);

    // Error message on fail, nullopt for success
    LoadScriptResult loadScript(const char* path, const std::vector<std::string>& flags);
    void addScript(LuaScript* script);
    void runUpdate(float dt) const;
    void runReady() const;
};
