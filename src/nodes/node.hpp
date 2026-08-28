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
    Node();
    virtual ~Node() = default;

    virtual void onTreeEnter(Scene* newScene) {}
    virtual void onTreeExit(Scene* currentScene) {}

    void addChild(std::unique_ptr<Node> child);
    void removeChild(const Node* child);
    
    LoadScriptResult setScript(const char* path, const std::vector<std::string>& flags);

    [[nodiscard]] Node* getChild(const std::string& childName) const;
    [[nodiscard]] LuaScript* script() const;
    [[nodiscard]] Scene* getScene() const;
    [[nodiscard]] virtual Node* getParent() const;
    [[nodiscard]] const std::vector<std::unique_ptr<Node>>& getChildren() const;
    [[nodiscard]] const std::string& getName() const;

    void setName(const std::string& newName);
    void setScene(Scene* newScene);

protected:
    Scene* scene = nullptr;
    Node* parent = nullptr;

    std::vector<std::unique_ptr<Node>> children;
    std::string name;

    std::unique_ptr<LuaScript> script_;
};
