#include "lua_event_bindings.hpp"

#include <lua.hpp>

#include <cstdint>
#include <exception>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

#include "../../util/event_bus.hpp"
#include "../../util/util.hpp"

namespace {

using Util::Event;
using Util::EventBus;
using Util::EventData;
using Util::EventValue;

struct LuaEvent final : Event {
};

EventBus* getEventBus(lua_State* L) {
    auto* eventBus = static_cast<EventBus*>(lua_touserdata(L, lua_upvalueindex(1)));
    if (!eventBus)
        luaL_error(L, "EventBus is not available");
    return eventBus;
}

const void* getOwner(lua_State* L) {
    return lua_touserdata(L, lua_upvalueindex(2));
}

void pushEventValue(lua_State* L, const EventValue& value) {
    std::visit([L](const auto& item) {
        using Value = std::decay_t<decltype(item)>;
        if constexpr (std::is_same_v<Value, bool>) {
            lua_pushboolean(L, item);
        } else if constexpr (std::is_same_v<Value, std::int64_t>) {
            lua_pushinteger(L, static_cast<lua_Integer>(item));
        } else if constexpr (std::is_same_v<Value, double>) {
            lua_pushnumber(L, item);
        } else if constexpr (std::is_same_v<Value, std::string>) {
            lua_pushlstring(L, item.data(), item.size());
        }
    }, value);
}

void pushEventTable(lua_State* L, const std::string& eventName, const Event& event) {
    lua_createtable(L, 0, static_cast<int>(event.data.size() + 1));
    for (const auto& [key, value] : event.data) {
        pushEventValue(L, value);
        lua_pushlstring(L, key.data(), key.size());
        lua_insert(L, -2);
        lua_settable(L, -3);
    }
    lua_pushlstring(L, eventName.data(), eventName.size());
    lua_setfield(L, -2, "name");
}

EventData readEventData(lua_State* L, const int index) {
    EventData data;
    if (lua_isnoneornil(L, index))
        return data;
    luaL_checktype(L, index, LUA_TTABLE);

    const int tableIndex = lua_absindex(L, index);
    lua_pushnil(L);
    while (lua_next(L, tableIndex) != 0) {
        if (lua_type(L, -2) != LUA_TSTRING) {
            lua_pop(L, 2);
            luaL_error(L, "Event payload keys must be strings");
        }

        size_t keyLength = 0;
        const char* keyData = lua_tolstring(L, -2, &keyLength);
        std::string key(keyData, keyLength);
        if (key == "name") {
            lua_pop(L, 2);
            luaL_error(L, "Event payload field 'name' is reserved");
        }

        switch (lua_type(L, -1)) {
            case LUA_TBOOLEAN:
                data.emplace(std::move(key), lua_toboolean(L, -1) != 0);
                break;
            case LUA_TNUMBER:
                if (lua_isinteger(L, -1)) {
                    data.emplace(std::move(key), static_cast<std::int64_t>(lua_tointeger(L, -1)));
                } else {
                    data.emplace(std::move(key), static_cast<double>(lua_tonumber(L, -1)));
                }
                break;
            case LUA_TSTRING: {
                size_t valueLength = 0;
                const char* valueData = lua_tolstring(L, -1, &valueLength);
                data.emplace(std::move(key), std::string(valueData, valueLength));
                break;
            }
            default:
                lua_pop(L, 2);
                luaL_error(L, "Event payload values must be booleans, numbers, or strings");
        }

        lua_pop(L, 1);
    }

    return data;
}

int eventBusSubscribe(lua_State* L) {
    EventBus* eventBus = getEventBus(L);
    size_t nameLength = 0;
    const char* name = luaL_checklstring(L, 1, &nameLength);
    luaL_checktype(L, 2, LUA_TFUNCTION);
    const std::string eventName(name, nameLength);
    const void* owner = getOwner(L);

    lua_pushvalue(L, 2);
    const int callbackRef = luaL_ref(L, LUA_REGISTRYINDEX);
    try {
        const auto id = eventBus->subscribe<Event>(
            eventName,
            [L, callbackRef, eventName](const Event& event) {
                const int originalTop = lua_gettop(L);
                lua_rawgeti(L, LUA_REGISTRYINDEX, callbackRef);
                pushEventTable(L, eventName, event);

                if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
                    const char* error = lua_tostring(L, -1);
                    Util::Log::error(error ? error : "Lua EventBus subscriber failed");
                }
                lua_settop(L, originalTop);
            },
            owner,
            [L, callbackRef] {
                luaL_unref(L, LUA_REGISTRYINDEX, callbackRef);
            }
        );
        lua_pushinteger(L, static_cast<lua_Integer>(id));
        return 1;
    } catch (const std::exception& error) {
        luaL_unref(L, LUA_REGISTRYINDEX, callbackRef);
        return luaL_error(L, "EventBus.subscribe failed: %s", error.what());
    }
}

int eventBusEmit(lua_State* L) {
    EventBus* eventBus = getEventBus(L);
    size_t nameLength = 0;
    const char* name = luaL_checklstring(L, 1, &nameLength);
    LuaEvent event;
    event.data = readEventData(L, 2);
    try {
        eventBus->emit(std::string(name, nameLength), event);
    } catch (const std::exception& error) {
        return luaL_error(L, "EventBus.emit failed: %s", error.what());
    }
    return 0;
}

int eventBusUnsubscribe(lua_State* L) {
    EventBus* eventBus = getEventBus(L);
    const auto id = static_cast<EventBus::SubscriptionId>(luaL_checkinteger(L, 1));
    lua_pushboolean(L, eventBus->unsubscribe(id));
    return 1;
}

void pushEventBusFunction(
    lua_State* L,
    const lua_CFunction function,
    Util::EventBus& eventBus,
    const void* owner
) {
    lua_pushlightuserdata(L, &eventBus);
    lua_pushlightuserdata(L, const_cast<void*>(owner));
    lua_pushcclosure(L, function, 2);
}

} // namespace

void LuaEventBindings::pushApi(lua_State* L, Util::EventBus& eventBus, const void* owner) {
    lua_createtable(L, 0, 3);

    pushEventBusFunction(L, eventBusSubscribe, eventBus, owner);
    lua_setfield(L, -2, "subscribe");

    pushEventBusFunction(L, eventBusEmit, eventBus, owner);
    lua_setfield(L, -2, "emit");

    pushEventBusFunction(L, eventBusUnsubscribe, eventBus, owner);
    lua_setfield(L, -2, "unsubscribe");
}

void LuaEventBindings::registerApi(lua_State* L, const LuaApiContext& context) {
    if (!context.eventBus)
        throw std::invalid_argument("EventBus API requires an event bus");

    pushApi(L, *context.eventBus, L);
    lua_setglobal(L, "EventBus");
}
