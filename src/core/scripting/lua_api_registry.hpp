#pragma once

#include <vector>

#include "lua_api_context.hpp"

struct lua_State;

class LuaApiRegistry {
public:
    using BindingModule = void (*)(lua_State* L, const LuaApiContext& context);

    static LuaApiRegistry createDefault();

    void addModule(BindingModule module);
    void registerAll(lua_State* L, const LuaApiContext& context) const;

private:
    std::vector<BindingModule> modules_;
};
