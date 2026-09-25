#pragma once

#include "../../camera/camera.hpp"
#include "../lua_api_context.hpp"

struct lua_State;

class LuaCamera {
private:
    Camera* cam;

public:
    LuaCamera(Camera* cam);
    
    void setPosition(glm::vec3 pos) const;
    glm::vec3 getPosition() const;

    void setYaw(float yaw) const;
    void setPitch(float pitch) const;
    float getYaw() const;
    float getPitch() const;
};

namespace LuaCameraBindings {
    void registerCamera(lua_State* L, Camera* cam);
    void registerApi(lua_State* L, const LuaApiContext& context);
}