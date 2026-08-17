#pragma once

#include <string>
#include <vector>
#include <algorithm>

class Scene;

class Node {
public:
    Node() = default;
    virtual ~Node() = default;

    virtual void onTreeEnter(Scene* scene) {}
    virtual void onTreeExit(Scene* scene) {}

    void addChild(Node* child) {
        this->children.push_back(child);

        child->onTreeEnter(this->scene);
    }

    void removeChild(const Node* child) {
        const auto it = std::find(
            this->children.begin(),
            this->children.end(),
            child
            );

        if (it != children.end()) {
            delete *it;
            children.erase(it);
        }
    }

    [[nodiscard]] Scene* getScene() const {
        return scene;
    }

    [[nodiscard]] virtual Node* getParent() const {
        return this->parent;
    }

    [[nodiscard]] const std::vector<Node*>& getChildren() const {
        return this->children;
    }

    [[nodiscard]] const std::string& getName() const {
        return this->name;
    }

    void setName(const std::string& name) {
        this->name = name;
    }

    void setScene(Scene* scene) {
        this->scene = scene;

        this->onTreeEnter(scene);

        for (Node* child : this->children) {
            child->setScene(scene);
        }
    }

protected:
    Scene* scene = nullptr;
    Node* parent = nullptr;

    std::vector<Node*> children;
    std::string name;
};
