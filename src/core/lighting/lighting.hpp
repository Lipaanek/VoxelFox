// This class is for managing lighting inside a scene
#pragma once

#include "light.hpp"

class Lighting {
private:
    float shininess = 32.0f;
    glm::vec3 skyColor { 0.3f, 0.35f, 0.45f };
    glm::vec3 groundColor { 0.08f, 0.08f, 0.10f };
public:
    Lighting() {}
    ~Lighting() = default;

    void setShininess(float shininess);
    void setSkyColor(glm::vec3 color);
    void setGroundColor(glm::vec3 color);
    void upload(ShaderProgram& program);

    float getShininess() const;
    glm::vec3 getSkyColor() const;
    glm::vec3 getGroundColor() const;

    SceneLights lights;
};