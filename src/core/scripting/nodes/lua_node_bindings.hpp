#pragma once

#include <cstdint>

struct lua_State;

class Node;

namespace LuaNodeBindings {
    void pushNodePrototype(lua_State* L, Node& node);
}
