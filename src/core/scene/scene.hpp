#pragma once

#include <memory>

#include "../../nodes/node.hpp"
#include "../lighting/lighting.hpp"
#include "../renderer/mesh/mesh_manager.hpp"
#include "core/util/util.hpp"

class Scene {
private:
    std::unique_ptr<Node> root = nullptr;
    Lighting lighting;
    MeshManager meshManager;
    LuaEngine lua_;

public:
    virtual ~Scene() = default;

    void setRoot(std::unique_ptr<Node> newRoot) {
        root = std::move(newRoot);

        if (root) {
            root->setScene(this);
        }
    }

    Node* getRoot() const { return this->root.get(); }

    LuaEngine& lua() { return lua_; }

    virtual void update(float dt) {
        onUpdate(dt);
        lua_.runUpdate(dt);
    }

    void ready() {
        Util::Log::log("Ready scene");
        onReady();
        lua_.runReady();
    }

    MeshManager& getMeshManager() {
        return this->meshManager;
    }
    Lighting& getLighting() {
        return this->lighting;
    }

protected:
    virtual void onReady() {}
    virtual void onUpdate(float dt) {}
};
