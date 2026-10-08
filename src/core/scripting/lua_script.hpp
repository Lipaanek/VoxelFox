#pragma once

#include "lua_engine.hpp"
#include <lua.hpp>

class Node;
namespace Util {
    class EventBus;
}

class LuaScript {
private:
    LuaEngine& engine_;
    class Node* attachedNode_ = nullptr;
    Util::EventBus* eventBus_ = nullptr;
    int environmentRef_ = LUA_NOREF;

    void releaseEnvironment();
    void callLifecycle(const char* name, float dt, bool passDelta) const;

public:
    explicit LuaScript(LuaEngine& engine);
    LuaScript(LuaEngine& engine, Node& node);
    ~LuaScript();

    LuaScript(const LuaScript&) = delete;
    LuaScript& operator=(const LuaScript&) = delete;

    LoadScriptResult load(const char* path) const;

    LoadScriptResult setScript(const char* path, const std::vector<std::string>& flags);

    void ready() const;
    void update(float dt) const;
};
