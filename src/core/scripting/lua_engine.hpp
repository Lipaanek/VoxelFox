#pragma once

struct lua_State;

class LuaEngine {
public:
    LuaEngine();
    ~LuaEngine();

    LuaEngine(const LuaEngine&) = delete;
    LuaEngine& operator=(const LuaEngine&) = delete;

    lua_State* state();
    bool loadScript(const char* path);
    void runUpdate(float dt);
    void runOnReady();

private:
    lua_State* L;
};
