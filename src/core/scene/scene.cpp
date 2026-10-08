#include "scene.hpp"

#include "../events/node_events.hpp"
#include "../../nodes/node3d.hpp"
#include "../../nodes/ui/ui_node.hpp"

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
    eventBus_.subscribe<UINodeSetPositionEvent>(
        "node.set_position",
        [this](const UINodeSetPositionEvent& event) {
            auto* node = dynamic_cast<UINode*>(findNode(event.nodeId));
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
