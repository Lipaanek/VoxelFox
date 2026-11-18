#pragma once

#include "../../camera/camera.hpp"
#include "../lua_api_context.hpp"

struct lua_State;

class LuaCamera {
private:
    Camera* m_cam;

public:
    explicit LuaCamera(Camera* cam);
    
    void setPosition(glm::vec3 pos) const;
    [[nodiscard]] glm::vec3 getPosition() const;

    void setYaw(float yaw) const;
    void setPitch(float pitch) const;
    [[nodiscard]] float getYaw() const;
    [[nodiscard]] float getPitch() const;
};

namespace LuaCameraBindings {
    void registerCamera(lua_State* L, Camera* cam);
    void registerApi(lua_State* L, const LuaApiContext& context);
}