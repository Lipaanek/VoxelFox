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
    eventBus_.subscribe<NodeSetPositionEvent>(
        "node.set_position",
        [this](const NodeSetPositionEvent& event) {
            auto* node = dynamic_cast<Node3D*>(findNode(event.nodeId));
            if (node)
                node->setPosition(event.position);
        },
        this
    );
}

Scene::~Scene() {
    root.reset();
    eventBus_.unsubscribeOwner(this);
}

Node* Scene::findNode(const std::int64_t instanceId) const {
    return findNodeRecursive(root.get(), instanceId);
}

void Scene::addUIElement(UIElement element) {
    uiElements.push_back(std::move(element));
}

std::vector<UIElement>& Scene::getUIElements() {
    return uiElements;
}

const std::vector<UIElement>& Scene::getUIElements() const {
    return uiElements;
}
