#pragma once

#include <glm/glm.hpp>

class Camera {
private:
    glm::vec3 m_position { 0.0f, 0.0f, 3.0f };
    float m_yaw = -90.0f;
    float m_pitch = 0.0f;
    float m_fov = 45.0f;
    float m_near = 0.1f;
    float m_far = 100.0f;

public:
    Camera() = default;
    ~Camera() = default;

    [[nodiscard]] glm::vec3 getFront() const;
    [[nodiscard]] glm::vec3 getRight() const;
    [[nodiscard]] glm::vec3 getUp() const;
    [[nodiscard]] glm::mat4 getViewMatrix() const;
    [[nodiscard]] glm::mat4 getProjectionMatrix(float aspect) const;

    void setPosition(glm::vec3 position);
    void setYaw(float yaw);
    void setPitch(float pitch);
    void setFov(float fov);
    void setClippingPlanes(float nearPlane, float farPlane);

    [[nodiscard]] glm::vec3 getPosition() const;
    [[nodiscard]] float getYaw() const;
    [[nodiscard]] float getPitch() const;
    [[nodiscard]] float getFov() const;
    [[nodiscard]] float getNearPlane() const;
    [[nodiscard]] float getFarPlane() const;
};
