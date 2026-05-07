#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Camera {
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    
    float yaw = -90.0f;
    float pitch = 0.0f;
    float zoom = 45.0f; // FOV
    
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    
    Camera() = default;
    Camera(glm::vec3 pos, glm::vec3 target, glm::vec3 upVec) {
        position = pos;
        front = glm::normalize(target - pos);
        right = glm::normalize(glm::cross(front, upVec));
        this->up = glm::cross(right, front);
    }
    
    glm::mat4 getViewMatrix() const {
        return glm::lookAt(position, position + front, up);
    }
    
    glm::mat4 getProjectionMatrix(float aspectRatio) const {
        return glm::perspective(glm::radians(zoom), aspectRatio, nearPlane, farPlane);
    }
    
    void updateCameraVectors() {
        // Calculate front from yaw/pitch
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(front);
        
        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }

    void updateCameraPosition(float vx, float vy, float vz) {
        position.x += vx;
        position.y += vy;
        position.z += vz;
    }
};

#endif