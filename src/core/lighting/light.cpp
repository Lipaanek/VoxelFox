#include "light.hpp"
#include <vector>
#include "../util/util.hpp"

size_t SceneLights::addLight(const Light& light) {
    if (this->m_lightCount >= m_kMaxLights) {
        Util::Log::error("SceneLights: maximum lights reached.");
        return this->m_lightCount;
    }

    this->m_sceneLights[m_lightCount] = light;
    return this->m_lightCount++;
}

void SceneLights::clear() {
    m_lightCount = 0;
}

void SceneLights::removeLight(const Light& light) {
    for (size_t i = 0; i < m_lightCount; i ++) {
        if (&m_sceneLights[i] == &light) {
            m_sceneLights[i] = m_sceneLights[--m_lightCount];
            return;
        }
    }
}

void SceneLights::uploadLights(ShaderProgram& program) const {
    program.setUniform("u_lightCount", static_cast<int>(m_lightCount));

    std::vector<int> types;
    std::vector<glm::vec3> positions, directions, colors;
    std::vector<float> energies, ranges;
    
    for (const auto&[type, direction, color, energy, range, position] : this->m_sceneLights) {
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
    for (size_t i = 0; i < m_lightCount; i ++) {
        if (&m_sceneLights[i] == &light) {
            m_sceneLights[i] = light;
            return;
        }
    }
}

Light& SceneLights::getLight(const size_t index) {
    return m_sceneLights[index];
}