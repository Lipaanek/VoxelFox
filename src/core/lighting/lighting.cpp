#include "lighting.hpp"

void Lighting::setShininess(float shininess) {
    this->m_shininess = shininess;
}

void Lighting::setSkyColor(const glm::vec3 color) {
    this->m_skyColor = color;
}

void Lighting::setGroundColor(const glm::vec3 color) {
    this->m_groundColor = color;
}

float Lighting::getShininess() const {
    return this->m_shininess;
}

glm::vec3 Lighting::getSkyColor() const {
    return this->m_skyColor;
}

glm::vec3 Lighting::getGroundColor() const {
    return this->m_groundColor;
}

void Lighting::upload(ShaderProgram& program) {
    this->lights.uploadLights(program);
    
    program.setUniform("u_shininess", this->m_shininess);
    program.setUniform("u_skyColor", this->m_skyColor);
    program.setUniform("u_groundColor", this->m_groundColor);
}