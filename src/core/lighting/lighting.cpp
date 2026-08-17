#include "lighting.hpp"

void Lighting::setShininess(float shininess) {
    this->shininess = shininess;
}

void Lighting::setSkyColor(const glm::vec3 color) {
    this->skyColor = color;
}

void Lighting::setGroundColor(const glm::vec3 color) {
    this->groundColor = color;
}

float Lighting::getShininess() const {
    return this->shininess;
}

glm::vec3 Lighting::getSkyColor() const {
    return this->skyColor;
}

glm::vec3 Lighting::getGroundColor() const {
    return this->groundColor;
}

void Lighting::upload(ShaderProgram& program) {
    this->lights.uploadLights(program);
    
    program.setUniform("u_shininess", this->shininess);
    program.setUniform("u_skyColor", this->skyColor);
    program.setUniform("u_groundColor", this->groundColor);
}