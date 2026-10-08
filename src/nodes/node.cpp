#include "node.hpp"

#include <algorithm>
#include <atomic>
#include <utility>

#include "../core/scene/scene.hpp"
#include "../core/util/util.hpp"

namespace {
std::atomic<std::int64_t> nextNodeInstanceId { 1 };
}

Node::Node()
    : instanceId(nextNodeInstanceId.fetch_add(1, std::memory_order_relaxed)) {
    this->setName("Node");
}

Node::~Node() = default;

void Node::removeChild(const Node *child) {
    const auto it = std::find_if(
            children.begin(),
            children.end(),
            [child](const std::unique_ptr<Node>& node) {
                return node.get() == child;
            }
        );

    if (it != children.end()) {
        children.erase(it);
    }
}

LuaScript *Node::script() const {
    return script_.get();
}

Scene *Node::getScene() const {
    return scene;
}

Node *Node::getParent() const {
    return this->parent;
}

const std::vector<std::unique_ptr<Node> > &Node::getChildren() const {
    return children;
}

const std::string &Node::getName() const {
    return this->name;
}

std::int64_t Node::getInstanceId() const {
    return this->instanceId;
}

void Node::setName(const std::string &newName) {
    this->name = newName;
}

void Node::setScene(Scene* newScene) {
    this->scene = newScene;
    this->onTreeEnter(newScene);

    for (const auto& child : this->children) {
        child->setScene(newScene);
    }
}

void Node::addChild(std::unique_ptr<Node> child) {
    if (!child) {
        return;
    }

    child->parent = this;

    if (scene) {
        child->setScene(scene);
    }

    children.push_back(std::move(child));
}

LoadScriptResult Node::setScript(const char* path, const std::vector<std::string>& flags) {
    if (!scene) {
        constexpr const char* message = "Cannot set script: Node has no Scene";
        Util::Log::error(message);
        return { .result = LoadResult::Failed, .message = message };
    }

    script_ = std::make_unique<LuaScript>(scene->lua(), *this);

    auto result = script_->setScript(path, flags);

    if (result.result == LoadResult::Success) {
        scene->lua().addScript(script_.get());
    } else {
        Util::Log::error(result.message);
        script_.reset();
    }

    return result;
}

Node* Node::getChild(const std::string& childName) const {
    for (const auto& child : children) {
        if (child->getName() == childName) {
            return child.get();
        }
    }

    return nullptr;
}
