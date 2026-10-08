#pragma once

#include "../lua_api_context.hpp"

struct lua_State;

namespace Util {
    class EventBus;
}

namespace LuaEventBindings {
    void pushApi(lua_State* L, Util::EventBus& eventBus, const void* owner);
    void registerApi(lua_State* L, const LuaApiContext& context);
}
