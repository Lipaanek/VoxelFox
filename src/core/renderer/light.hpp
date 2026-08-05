#pragma once

#include <glm/glm.hpp>
#include "shader_program.hpp"

enum class LightType { Directional, Point };

struct Light {
    LightType type = LightType::Point;
    glm::vec3 position { 0.0f };
    glm::vec3 direction { 0.0f, -1.0f, 0.0f }; // Only for directional light
    glm::vec3 color { 1.0f };
    float energy = 1.0f; // total light output
    float range = 10.0f; // distance at which point light fades to zero
};

class SceneLights {
private:
    std::vector<Light> sceneLights;
    static constexpr int kMaxLights = 8;
public:
    SceneLights() {}
    ~SceneLights() = default;

    size_t addLight(const Light& light);
    void clear();
    void uploadLights(ShaderProgram& program);
    Light& getLight(size_t index);
};