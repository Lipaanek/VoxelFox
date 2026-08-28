#pragma once

#include <glm/glm.hpp>
#include "node3d.hpp"
#include "../core/lighting/light.hpp"

class Light3D : public Node3D {
private:
    Light light;

public:
    Light3D();

    void updateLight();

    void onTreeEnter(Scene *newScene) override;
    void onTreeExit(Scene *currentScene) override;

    void setLightType(LightType type);
    void setLightPosition(glm::vec3 position);
    void setLightDirection(glm::vec3 direction);
    void setColor(glm::vec3 color);
    void setEnergy(float energy);
    void setRange(float range);

    [[nodiscard]] glm::vec3 getLightPosition() const;
    [[nodiscard]] glm::vec3 getColor() const;
    [[nodiscard]] float getEnergy() const;
    [[nodiscard]] float getRange() const;
};

