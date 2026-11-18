#include "camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

glm::vec3 Camera::getFront() const {
    glm::vec3 front;

    front.x = cos(glm::radians(this->m_yaw)) * cos(glm::radians(this->m_pitch));
    front.y = sin(glm::radians(this->m_pitch));
    front.z = sin(glm::radians(this->m_yaw)) * cos(glm::radians(this->m_pitch));

    return front;
}

glm::vec3 Camera::getRight() const {
    return glm::normalize(
        glm::cross(getFront(), glm::vec3(0.0f, 1.0f, 0.0f))
    );
}

glm::vec3 Camera::getUp() const {
    return glm::normalize(
        glm::cross(getRight(), getFront())
    );
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(this->m_position, this->m_position + this->getFront(), glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 Camera::getProjectionMatrix(const float aspect) const {
    return glm::perspective(glm::radians(this->m_fov), aspect, this->m_near, this->m_far);
}

void Camera::setPosition(glm::vec3 position) { this->m_position = position; }
void Camera::setYaw(float yaw) { this->m_yaw = yaw; }
void Camera::setPitch(float pitch) { this->m_pitch = pitch; }
void Camera::setFov(float fov) { this->m_fov = fov; }
void Camera::setClippingPlanes(float nearPlane, float farPlane) { this->m_near = nearPlane; this->m_far = farPlane; }

glm::vec3 Camera::getPosition() const { return this->m_position; }
float Camera::getYaw() const { return this->m_yaw; }
float Camera::getPitch() const { return this->m_pitch; }
float Camera::getFov() const { return this->m_fov; }
float Camera::getNearPlane() const { return this->m_near; }
float Camera::getFarPlane() const { return this->m_far; }
