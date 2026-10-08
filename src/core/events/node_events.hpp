#pragma once

#include <cstdint>

#include <glm/glm.hpp>

#include "../util/event.hpp"

struct NodeSetPositionEvent : Util::Event {
    std::int64_t nodeId;
    glm::vec3 position;

    NodeSetPositionEvent(const std::int64_t nodeId, const glm::vec3 position)
        : nodeId(nodeId), position(position) {
        data.emplace("node_id", nodeId);
        data.emplace("x", static_cast<double>(position.x));
        data.emplace("y", static_cast<double>(position.y));
        data.emplace("z", static_cast<double>(position.z));
    }
};

struct UINodeSetPositionEvent : Util::Event {
    std::int64_t nodeId;
    glm::vec2 position;

    UINodeSetPositionEvent(const std::int64_t nodeId, const glm::vec2 position)
        : nodeId(nodeId), position(position) {
        data.emplace("node_id", nodeId);
        data.emplace("x", static_cast<double>(position.x));
        data.emplace("y", static_cast<double>(position.y));
    }
};
