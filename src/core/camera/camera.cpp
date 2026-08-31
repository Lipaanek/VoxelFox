#include "camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

glm::vec3 Camera::getFront() const {
    glm::vec3 front;

    front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    front.y = sin(glm::radians(this->pitch));
    front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

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
    return glm::lookAt(this->position, this->position + this->getFront(), glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 Camera::getProjectionMatrix(const float aspect) const {
    return glm::perspective(glm::radians(this->fov), aspect, this->near, this->far);
}

void Camera::setPosition(glm::vec3 position) { this->position = position; }
void Camera::setYaw(float yaw) { this->yaw = yaw; }
void Camera::setPitch(float pitch) { this->pitch = pitch; }
void Camera::setFov(float fov) { this->fov = fov; }
void Camera::setClippingPlanes(float nearPlane, float farPlane) { this->near = nearPlane; this->far = farPlane; }

glm::vec3 Camera::getPosition() const { return this->position; }
float Camera::getYaw() const { return this->yaw; }
float Camera::getPitch() const { return this->pitch; }
float Camera::getFov() const { return this->fov; }
float Camera::getNearPlane() const { return this->near; }
float Camera::getFarPlane() const { return this->far; }
