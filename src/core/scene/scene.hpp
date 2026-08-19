#pragma once

#include <memory>

#include "../../nodes/node.hpp"
#include "../lighting/lighting.hpp"
#include "../renderer/mesh/mesh_manager.hpp"

class Scene {
private:
    std::unique_ptr<Node> root = nullptr;
    Lighting lighting;
    MeshManager meshManager;
    LuaEngine lua_;

public:
    virtual ~Scene() = default;

    void setRoot(std::unique_ptr<Node> newRoot) {
        std::cout << "Scene this: " << this << std::endl;

        root = std::move(newRoot);

        if (root) {
            std::cout << "Setting root scene to: " << this << std::endl;
            root->setScene(this);
        }
    }

    Node* getRoot() const { return this->root.get(); }

    LuaEngine& lua() { return lua_; }

    virtual void update(float dt) = 0;
    virtual void ready() = 0;

    MeshManager& getMeshManager() {
        return this->meshManager;
    }
    Lighting& getLighting() {
        return this->lighting;
    }
};