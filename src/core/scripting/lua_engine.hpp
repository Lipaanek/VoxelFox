#pragma once

#include <string>
#include <vector>

#include "lua_flags.hpp"

struct lua_State;

class LuaEngine {
public:
    LuaEngine();
    ~LuaEngine();

    LuaEngine(const LuaEngine&) = delete;
    LuaEngine& operator=(const LuaEngine&) = delete;

    lua_State* state();
    bool loadScript(const char* path, const std::vector<std::string>& flags);
    void runUpdate(float dt);
    void runOnReady();
    bool hasFlag(const std::string& flag) const;

private:
    lua_State* L;
    LuaFlags flags_;
};
