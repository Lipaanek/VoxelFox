#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <memory>

#include "../core/scripting/lua_script.hpp"

class Scene;

class Node {
public:
    Node() {
        this->setName("Node");
    }

    virtual ~Node() = default;

    virtual void onTreeEnter(Scene* newScene) {}
    virtual void onTreeExit(Scene* currentScene) {}

    void addChild(std::unique_ptr<Node> child) {
        if (!child) {
            return;
        }

        child->parent = this;

        if (scene) {
            child->setScene(scene);
        }

        children.push_back(std::move(child));
    }

    [[nodiscard]] Node* getChild(const std::string& childName) const;

    void removeChild(const Node* child) {
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

    LoadScriptResult setScript(const char* path, const std::vector<std::string>& flags);

    [[nodiscard]] LuaScript* script() const {
        return script_.get();
    }

    [[nodiscard]] Scene* getScene() const {
        return scene;
    }

    [[nodiscard]] virtual Node* getParent() const {
        return this->parent;
    }

    [[nodiscard]] const std::vector<std::unique_ptr<Node>>& getChildren() const {
        return children;
    }

    [[nodiscard]] const std::string& getName() const {
        return this->name;
    }

    void setName(const std::string& newName) {
        this->name = newName;
    }

    void setScene(Scene* newScene) {
        this->scene = newScene;
        this->onTreeEnter(newScene);

        for (const auto& child : this->children) {
            child->setScene(newScene);
        }
    }

protected:
    Scene* scene = nullptr;
    Node* parent = nullptr;

    std::vector<std::unique_ptr<Node>> children;
    std::string name;

    std::unique_ptr<LuaScript> script_;
};
