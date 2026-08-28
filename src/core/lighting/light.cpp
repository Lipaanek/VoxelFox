#include "light.hpp"
#include <vector>
#include "../util/util.hpp"

size_t SceneLights::addLight(const Light& light) {
    if (this->count >= kMaxLights) {
        Util::Log::error("SceneLights: maximum lights reached.");
        return this->count;
    }

    this->sceneLights[count] = light;
    return this->count++;
}

void SceneLights::clear() {
    count = 0;
}

void SceneLights::removeLight(const Light& light) {
    for (size_t i = 0; i < count; i ++) {
        if (&sceneLights[i] == &light) {
            sceneLights[i] = sceneLights[--count];
            return;
        }
    }
}

void SceneLights::uploadLights(ShaderProgram& program) const {
    program.setUniform("u_lightCount", static_cast<int>(count));

    std::vector<int> types;
    std::vector<glm::vec3> positions, directions, colors;
    std::vector<float> energies, ranges;
    
    for (const auto&[type, position, direction, color, energy, range] : this->sceneLights) {
        types.push_back(type == LightType::Directional ? 0 : 1);
        positions.push_back(position);
        directions.push_back(direction);
        colors.push_back(color);
        energies.push_back(energy);
        ranges.push_back(range);
    }

    program.setUniform("u_lightTypes", types);
    program.setUniform("u_lightPositions", positions);
    program.setUniform("u_lightDirections", directions);
    program.setUniform("u_lightColors", colors);
    program.setUniform("u_lightEnergy", energies);
    program.setUniform("u_lightRanges", ranges);
}

void SceneLights::updateLight(Light &light) {
    for (size_t i = 0; i < count; i ++) {
        if (&sceneLights[i] == &light) {
            sceneLights[i] = light;
            return;
        }
    }
}

Light& SceneLights::getLight(const size_t index) {
    return sceneLights[index];
}