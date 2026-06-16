#pragma once

#include "node3d.hpp"
#include <glm/glm.hpp>

class Light3D : public Node3D {
public:
    glm::vec3 color{1.0f, 1.0f, 1.0f};
    float intensity = 1.0f;

    void BuildUI() override;

    std::string GetNodeType() const override { return "Light3D"; }
};
