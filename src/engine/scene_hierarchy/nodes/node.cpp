#include "node.hpp"
#include "../scene.hpp"
#include <algorithm>

Node::~Node() {
    OnDestroy();
}

void Node::AddChild(std::unique_ptr<Node> child) {
    child->parent = this;
    SetSceneRecursive(child.get(), scene);
    child->OnCreate();
    child->OnEnterTree();
    for (auto& c : child->children) {
        SetSceneRecursive(c.get(), scene);
        PropagateEnterTree(c.get());
    }
    children.push_back(std::move(child));
}

std::unique_ptr<Node> Node::RemoveChild(Node* child) {
    for (auto it = children.begin(); it != children.end(); ++it) {
        if (it->get() == child) {
            std::unique_ptr<Node> result = std::move(*it);
            children.erase(it);
            PropagateExitTree(result.get());
            SetSceneRecursive(result.get(), nullptr);
            result->parent = nullptr;
            result->ready_ = false;
            return result;
        }
    }
    return nullptr;
}

Node* Node::GetChild(const std::string& name) {
    for (auto& child : children) {
        if (child->name == name) return child.get();
    }
    return nullptr;
}

std::vector<Node*> Node::GetChildren() {
    std::vector<Node*> result;
    result.reserve(children.size());
    for (auto& child : children) {
        result.push_back(child.get());
    }
    return result;
}

void Node::QueueFree() {
    queuedForDeletion = true;
    if (scene) {
        scene->QueueNodeForDeletion(this);
    }
}

void Node::SetSceneRecursive(Node* node, Scene* newScene) {
    node->scene = newScene;
    for (auto& child : node->children) {
        SetSceneRecursive(child.get(), newScene);
    }
}

void Node::PropagateEnterTree(Node* node) {
    node->OnEnterTree();
    for (auto& child : node->children) {
        PropagateEnterTree(child.get());
    }
}

void Node::PropagateExitTree(Node* node) {
    node->OnExitTree();
    for (auto& child : node->children) {
        PropagateExitTree(child.get());
    }
}
