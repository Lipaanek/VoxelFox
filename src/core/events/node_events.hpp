#pragma once

#include <cstdint>

#include <glm/glm.hpp>

#include "../util/event.hpp"

struct NodeSetPositionEvent : Util::Event {
    std::int64_t m_nodeId;
    glm::vec3 m_position;

    NodeSetPositionEvent(const std::int64_t nodeId, const glm::vec3 position)
        : m_nodeId(nodeId), m_position(position) {
        m_data.emplace("node_id", nodeId);
        m_data.emplace("x", static_cast<double>(position.x));
        m_data.emplace("y", static_cast<double>(position.y));
        m_data.emplace("z", static_cast<double>(position.z));
    }
};
