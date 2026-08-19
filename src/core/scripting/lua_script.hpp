#pragma once

#include "lua_engine.hpp"
#include <lua.hpp>

class LuaScript {
private:
    LuaEngine& engine_;
    int instanceRef_ = LUA_NOREF;

public:
    explicit LuaScript(LuaEngine& engine);
    ~LuaScript() = default;

    LuaScript(const LuaScript&) = delete;
    LuaScript& operator=(const LuaScript&) = delete;

    LoadScriptResult load(const char* path) const;

    LoadScriptResult setScript(const char* path, const std::vector<std::string>& flags);

    void ready() const;
    void update(float dt) const;
};
