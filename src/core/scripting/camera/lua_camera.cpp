#include "lua_camera.hpp"

#include <lua.hpp>

#include "../vector/lua_vector3.hpp"

namespace {

constexpr const char* CAMERA_REGISTRY_KEY = "voxelfox_camera";

Camera* getCamera(lua_State* L) {
    lua_getfield(L, LUA_REGISTRYINDEX, CAMERA_REGISTRY_KEY);
    Camera* cam = static_cast<Camera*>(lua_touserdata(L, -1));
    lua_pop(L, 1);

    if (!cam)
        luaL_error(L, "Camera is not registered");
    return cam;
}

int cameraSetPosition(lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_getfield(L, 1, "x");
    lua_getfield(L, 1, "y");
    lua_getfield(L, 1, "z");
    glm::vec3 pos {
        static_cast<float>(luaL_checknumber(L, -3)),
        static_cast<float>(luaL_checknumber(L, -2)),
        static_cast<float>(luaL_checknumber(L, -1))
    };
    lua_pop(L, 3);
    LuaCamera cam(getCamera(L));
    cam.setPosition(pos);
    return 0;
}

int cameraGetPosition(lua_State* L) {
    LuaCamera cam(getCamera(L));
    glm::vec3 pos = cam.getPosition();
    LuaVector3Bindings::pushVector3(L, pos.x, pos.y, pos.z);
    return 1;
}

int cameraSetYaw(lua_State* L) {
    LuaCamera cam(getCamera(L));
    cam.setYaw(static_cast<float>(luaL_checknumber(L, 1)));
    return 0;
}

int cameraSetPitch(lua_State* L) {
    LuaCamera cam(getCamera(L));
    cam.setPitch(static_cast<float>(luaL_checknumber(L, 1)));
    return 0;
}

int cameraGetYaw(lua_State* L) {
    LuaCamera cam(getCamera(L));
    lua_pushnumber(L, cam.getYaw());
    return 1;
}

int cameraGetPitch(lua_State* L) {
    LuaCamera cam(getCamera(L));
    lua_pushnumber(L, cam.getPitch());
    return 1;
}

const luaL_Reg cameraFunctions[] = {
    { "set_position", cameraSetPosition },
    { "get_position", cameraGetPosition },
    { "set_yaw", cameraSetYaw },
    { "set_pitch", cameraSetPitch },
    { "get_yaw", cameraGetYaw },
    { "get_pitch", cameraGetPitch },
    { nullptr, nullptr }
};

} // namespace

LuaCamera::LuaCamera(Camera* cam) : cam(cam) {}

void LuaCamera::setPosition(glm::vec3 pos) {
    if (this->cam)
        this->cam->setPosition(pos);
}

glm::vec3 LuaCamera::getPosition() {
    if (this->cam)
        return this->cam->getPosition();
    return glm::vec3(0.0f);
}

void LuaCamera::setYaw(float yaw) {
    if (this->cam)
        this->cam->setYaw(yaw);
}

void LuaCamera::setPitch(float pitch) {
    if (this->cam)
        this->cam->setPitch(pitch);
}

float LuaCamera::getYaw() {
    if (this->cam)
        return this->cam->getYaw();
    return 0.0f;
}

float LuaCamera::getPitch() {
    if (this->cam)
        return this->cam->getPitch();
    return 0.0f;
}

void LuaCameraBindings::registerCamera(lua_State* L, Camera* cam) {
    lua_pushlightuserdata(L, cam);
    lua_setfield(L, LUA_REGISTRYINDEX, CAMERA_REGISTRY_KEY);

    luaL_newlib(L, cameraFunctions);
    lua_setglobal(L, "Camera");
}
