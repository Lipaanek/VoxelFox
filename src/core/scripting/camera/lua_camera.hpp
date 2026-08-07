#pragma once

#include "../../camera/camera.hpp"

struct lua_State;

class LuaCamera {
private:
    Camera* cam;

public:
    LuaCamera(Camera* cam);
    
    void setPosition(glm::vec3 pos);
    glm::vec3 getPosition();

    void setYaw(float yaw);
    void setPitch(float pitch);
    float getYaw();
    float getPitch();
};

namespace LuaCameraBindings {
    void registerCamera(lua_State* L, Camera* cam);
}