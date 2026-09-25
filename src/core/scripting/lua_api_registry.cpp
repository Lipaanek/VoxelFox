#include "lua_api_registry.hpp"

#include <stdexcept>

#include "camera/lua_camera.hpp"
#include "inputs/lua_input_bindings.hpp"
#include "vector/lua_vector3.hpp"

LuaApiRegistry LuaApiRegistry::createDefault() {
    LuaApiRegistry registry;
    registry.addModule(&LuaCameraBindings::registerApi);
    registry.addModule(&LuaInputBindings::registerApi);
    registry.addModule(&LuaVector3Bindings::registerApi);
    return registry;
}

void LuaApiRegistry::addModule(const BindingModule module) {
    if (!module)
        throw std::invalid_argument("Lua API binding module cannot be null");

    modules_.push_back(module);
}

void LuaApiRegistry::registerAll(lua_State* L, const LuaApiContext& context) const {
    for (const auto module : modules_)
        module(L, context);
}
