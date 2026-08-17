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

public:
    virtual ~Scene() = default;

    void setRoot(std::unique_ptr<Node> root) {
        this->root = std::move(root);
    }

    Node getRoot() const { return *this->root; }

    virtual void update(float dt) = 0;
    virtual void ready() = 0;

    MeshManager& getMeshManager() {
        return this->meshManager;
    }
    Lighting& getLighting() {
        return this->lighting;
    }
};