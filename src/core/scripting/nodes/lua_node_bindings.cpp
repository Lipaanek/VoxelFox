#include "lua_node_bindings.hpp"

#include <lua.hpp>

#include <cstdint>
#include <string>
#include <string_view>

#include "../../events/node_events.hpp"
#include "../../scene/scene.hpp"
#include "../../../nodes/node.hpp"
#include "../../../nodes/node3d.hpp"
#include "../../../nodes/ui/ui_node.hpp"
#include "../vector/lua_vector3.hpp"

namespace {

struct LuaNodeReference {
    Scene* scene;
    std::int64_t instanceId;
};

Node* resolveNode(lua_State* L) {
    auto* reference = static_cast<LuaNodeReference*>(
        lua_touserdata(L, lua_upvalueindex(1))
    );
    if (!reference || !reference->scene)
        luaL_error(L, "Node reference is not attached to a scene");

    Node* node = reference->scene->findNode(reference->instanceId);
    if (!node)
        luaL_error(L, "Node reference is no longer in the scene tree");
    return node;
}

void pushBoundFunction(
    lua_State* L,
    Scene& scene,
    const std::int64_t instanceId,
    const lua_CFunction function
) {
    auto* reference = static_cast<LuaNodeReference*>(
        lua_newuserdata(L, sizeof(LuaNodeReference))
    );
    reference->scene = &scene;
    reference->instanceId = instanceId;
    lua_pushcclosure(L, function, 1);
}

void pushNodeObject(lua_State* L, Node& node);

int nodeGetChild(lua_State* L) {
    Node* node = resolveNode(L);
    const char* name = luaL_checkstring(L, 1);
    Node* child = node->getChild(name);
    if (!child) {
        lua_pushnil(L);
        return 1;
    }
    pushNodeObject(L, *child);
    return 1;
}

int nodeGetRoot(lua_State* L) {
    Node* node = resolveNode(L);
    Node* root = node->getScene()->getRoot();
    if (!root) {
        lua_pushnil(L);
        return 1;
    }
    pushNodeObject(L, *root);
    return 1;
}

int nodeGetName(lua_State* L) {
    Node* node = resolveNode(L);
    const std::string& name = node->getName();
    lua_pushlstring(L, name.data(), name.size());
    return 1;
}

int nodeGetNode(lua_State* L) {
    Node* start = resolveNode(L);
    const char* rawPath = luaL_checkstring(L, 1);
    const std::string_view path(rawPath);
    if (path.empty())
        return luaL_error(L, "Node path cannot be empty");

    Node* current = path.front() == '/'
        ? start->getScene()->getRoot()
        : start;
    std::size_t segmentStart = path.front() == '/' ? 1 : 0;

    while (segmentStart <= path.size()) {
        const std::size_t separator = path.find('/', segmentStart);
        const std::size_t segmentEnd = separator == std::string::npos
            ? path.size()
            : separator;
        const std::string_view segment = path.substr(segmentStart, segmentEnd - segmentStart);

        if (!segment.empty() && segment != ".") {
            if (segment == "..") {
                current = current ? current->getParent() : nullptr;
            } else if (current) {
                if (current != current->getScene()->getRoot() || segment != current->getName())
                    current = current->getChild(std::string(segment));
            }
        }

        if (!current)
            return luaL_error(L, "Node path '%s' was not found", rawPath);
        if (separator == std::string::npos)
            break;
        segmentStart = separator + 1;
    }

    pushNodeObject(L, *current);
    return 1;
}

glm::vec3 readVector3(lua_State* L, const int index) {
    luaL_checktype(L, index, LUA_TTABLE);
    const int table = lua_absindex(L, index);
    lua_getfield(L, table, "x");
    lua_getfield(L, table, "y");
    lua_getfield(L, table, "z");
    const glm::vec3 result {
        static_cast<float>(luaL_checknumber(L, -3)),
        static_cast<float>(luaL_checknumber(L, -2)),
        static_cast<float>(luaL_checknumber(L, -1))
    };
    lua_pop(L, 3);
    return result;
}

glm::vec2 readVector2(lua_State* L, const int index) {
    luaL_checktype(L, index, LUA_TTABLE);
    const int table = lua_absindex(L, index);
    lua_getfield(L, table, "x");
    lua_getfield(L, table, "y");
    const glm::vec2 result {
        static_cast<float>(luaL_checknumber(L, -2)),
        static_cast<float>(luaL_checknumber(L, -1))
    };
    lua_pop(L, 2);
    return result;
}

int nodeSetPosition(lua_State* L) {
    Node* node = resolveNode(L);
    if (dynamic_cast<Node3D*>(node)) {
        const glm::vec3 position = readVector3(L, 1);
        const NodeSetPositionEvent event(node->getInstanceId(), position);
        node->getScene()->eventBus().emit("node.set_position", event);
        return 0;
    }
    if (dynamic_cast<UINode*>(node)) {
        const glm::vec2 position = readVector2(L, 1);
        const UINodeSetPositionEvent event(node->getInstanceId(), position);
        node->getScene()->eventBus().emit("node.set_position", event);
        return 0;
    }
    return luaL_error(L, "set_position is only available on Node3D and UINode nodes");
}

int nodeGetPosition(lua_State* L) {
    Node* node = resolveNode(L);
    if (const auto* node3D = dynamic_cast<Node3D*>(node)) {
        const glm::vec3 position = node3D->getPosition();
        LuaVector3Bindings::pushVector3(L, position.x, position.y, position.z);
        return 1;
    }
    if (const auto* uiNode = dynamic_cast<UINode*>(node)) {
        const glm::vec2 position = uiNode->getPosition();
        lua_createtable(L, 0, 2);
        lua_pushnumber(L, position.x);
        lua_setfield(L, -2, "x");
        lua_pushnumber(L, position.y);
        lua_setfield(L, -2, "y");
        return 1;
    }
    return luaL_error(L, "get_position is only available on Node3D and UINode nodes");
}

void setBoundFunction(
    lua_State* L,
    Scene& scene,
    const std::int64_t instanceId,
    const char* name,
    const lua_CFunction function
) {
    pushBoundFunction(L, scene, instanceId, function);
    lua_setfield(L, -2, name);
}

void pushNodePrototype(lua_State* L, Node& node) {
    Scene* scene = node.getScene();
    if (!scene)
        luaL_error(L, "Cannot create a Lua reference for a node outside a scene");

    const std::int64_t instanceId = node.getInstanceId();
    lua_createtable(L, 0, 7);
    setBoundFunction(L, *scene, instanceId, "get_child", nodeGetChild);
    setBoundFunction(L, *scene, instanceId, "get_node", nodeGetNode);
    setBoundFunction(L, *scene, instanceId, "get_root", nodeGetRoot);
    setBoundFunction(L, *scene, instanceId, "get_name", nodeGetName);
    setBoundFunction(L, *scene, instanceId, "set_position", nodeSetPosition);
    setBoundFunction(L, *scene, instanceId, "get_position", nodeGetPosition);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
}

void pushNodeObject(lua_State* L, Node& node) {
    pushNodePrototype(L, node);
    lua_newtable(L);
    lua_pushvalue(L, -2);
    lua_setmetatable(L, -2);
    lua_remove(L, -2);
}

} // namespace

void LuaNodeBindings::pushNodePrototype(lua_State* L, Node& node) {
    ::pushNodePrototype(L, node);
}
