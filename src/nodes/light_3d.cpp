#include "light_3d.hpp"

#include "../core/scene/scene.hpp"

Light3D::Light3D() {
    this->setName("Light3D");
}

void Light3D::onTreeEnter(Scene *newScene) {
    newScene->getLighting().lights.addLight(this->light);
}

void Light3D::onTreeExit(Scene *currentScene) {
    currentScene->getLighting().lights.removeLight(this->light);
}

void Light3D::updateLight() {
    if (Scene* scene = this->getScene()) {
        scene->getLighting().lights.updateLight(this->light);
    }
}

glm::vec3 Light3D::getColor() const {
    return this->light.color;
}

float Light3D::getEnergy() const {
    return this->light.energy;
}

glm::vec3 Light3D::getLightPosition() const {
    return this->light.position;
}

float Light3D::getRange() const {
    return this->light.range;
}

void Light3D::setColor(const glm::vec3 color) {
    this->light.color = color;

    updateLight();
}

void Light3D::setEnergy(const float energy) {
    this->light.energy = energy;

    updateLight();
}

void Light3D::setLightDirection(const glm::vec3 direction) {
    if (this->light.type == LightType::Point) return;
    this->light.direction = direction;

    updateLight();
}

void Light3D::setLightType(const LightType type) {
    this->light.type = type;

    updateLight();
}

void Light3D::setLightPosition(const glm::vec3 position) {
    this->light.position = position;
    this->setPosition(position);

    updateLight();
}

void Light3D::setRange(const float range) {
    this->light.range = range;

    updateLight();
}
