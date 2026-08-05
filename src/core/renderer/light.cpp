#include "light.hpp"
#include <vector>
#include "../util/util.hpp"

size_t SceneLights::addLight(const Light& light) {
    if (this->sceneLights.size() >= kMaxLights) {
        Util::Log::error("SceneLights: maximum lights reached.");
        return this->sceneLights.size() - 1;
    }
    this->sceneLights.push_back(light);
    return this->sceneLights.size() - 1;
}

void SceneLights::clear() {
    this->sceneLights.clear();
}

void SceneLights::uploadLights(ShaderProgram& program) {
    program.setUniform("u_lightCount", static_cast<int>(this->sceneLights.size()));

    std::vector<int> types;
    std::vector<glm::vec3> positions, directions, colors;
    std::vector<float> energies, ranges;
    
    for (const Light& l : this->sceneLights) {
        types.push_back(l.type == LightType::Directional ? 0 : 1);
        positions.push_back(l.position);
        directions.push_back(l.direction);
        colors.push_back(l.color);
        energies.push_back(l.energy);
        ranges.push_back(l.range);
    }

    program.setUniform("u_lightTypes", types);
    program.setUniform("u_lightPositions", positions);
    program.setUniform("u_lightDirections", directions);
    program.setUniform("u_lightColors", colors);
    program.setUniform("u_lightEnergy", energies);
    program.setUniform("u_lightRanges", ranges);
}

Light& SceneLights::getLight(size_t index) {
    return this->sceneLights.at(index);
}