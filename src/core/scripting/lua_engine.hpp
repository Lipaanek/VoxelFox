#pragma once

#include <string>
#include <vector>

#include "lua_flags.hpp"

struct lua_State;

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

public:
    LuaEngine();
    ~LuaEngine();

    LuaEngine(const LuaEngine&) = delete;
    LuaEngine& operator=(const LuaEngine&) = delete;

    lua_State* state() const;

    // Error message on fail, nullopt for success
    LoadScriptResult loadScript(const char* path, const std::vector<std::string>& flags);
    void runUpdate(float dt) const;
    void runReady() const;
    bool hasFlag(const std::string& flag) const;
};
