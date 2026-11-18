#include "scene.hpp"

#include <utility>

#include "../events/node_events.hpp"
#include "../../nodes/node3d.hpp"

namespace {

Node* findNodeRecursive(Node* node, const std::int64_t instanceId) {
    if (!node)
        return nullptr;
    if (node->getInstanceId() == instanceId)
        return node;

    for (const auto& child : node->getChildren()) {
        if (Node* found = findNodeRecursive(child.get(), instanceId))
            return found;
    }
    return nullptr;
}

}

Scene::Scene() {
    m_eventBus_.subscribe<NodeSetPositionEvent>(
        "node.set_position",
        [this](const NodeSetPositionEvent& event) {
            auto* node = dynamic_cast<Node3D*>(findNode(event.m_nodeId));
            if (node)
                node->setPosition(event.m_position);
        },
        this
    );
}

Scene::~Scene() {
    m_root.reset();
    m_eventBus_.unsubscribeOwner(this);
}

Node* Scene::findNode(const std::int64_t instanceId) const {
    return findNodeRecursive(m_root.get(), instanceId);
}

void Scene::addUIElement(UIElement element) {
    m_uiElements.push_back(std::move(element));
}

std::vector<UIElement>& Scene::getUIElements() {
    return m_uiElements;
}

const std::vector<UIElement>& Scene::getUIElements() const {
    return m_uiElements;
}
