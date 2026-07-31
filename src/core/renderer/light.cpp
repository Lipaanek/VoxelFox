#include "light.hpp"
#include <vector>
#include "../util/util.hpp"

void SceneLights::addLight(const Light& light) {
    if (this->sceneLights.size() >= kMaxLights) {
        Util::Log::error("SceneLights: maximum lights reached.");
        return;
    }
    this->sceneLights.push_back(light);
}

void SceneLights::clear() {
    this->sceneLights.clear();
}

void SceneLights::uploadLights(ShaderProgram& program) {
    program.setUniform("u_lightCount", static_cast<int>(this->sceneLights.size()));

    std::vector<int> types;
    std::vector<glm::vec3> positions, directions, colors, attenuations;
    std::vector<float> intensities;
    
    for (const Light& l : this->sceneLights) {
        types.push_back(l.type == LightType::Directional ? 0 : 1);
        positions.push_back(l.position);
        directions.push_back(l.direction);
        colors.push_back(l.color);
        attenuations.push_back(l.attenuation);
        intensities.push_back(l.intensity);
    }

    program.setUniform("u_lightTypes", types);
    program.setUniform("u_lightPositions", positions);
    program.setUniform("u_lightDirections", directions);
    program.setUniform("u_lightColors", colors);
    program.setUniform("u_lightAttenuations", attenuations);
    program.setUniform("u_lightIntensities", intensities);
}