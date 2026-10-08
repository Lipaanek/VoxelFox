#pragma once

#include <memory>
#include <cstdint>

#include "../../nodes/node.hpp"
#include "../lighting/lighting.hpp"
#include "../renderer/mesh/mesh_manager.hpp"
#include "core/util/util.hpp"
#include "core/util/event_bus.hpp"

class Scene {
private:
    Util::EventBus eventBus_;
    LuaEngine lua_;
    std::unique_ptr<Node> root = nullptr;
    Lighting lighting;
    MeshManager meshManager;

public:
    Scene();
    virtual ~Scene();

    void setRoot(std::unique_ptr<Node> newRoot) {
        root = std::move(newRoot);

        if (root) {
            root->setScene(this);
        }
    }

    Node* getRoot() const { return this->root.get(); }

    LuaEngine& lua() { return lua_; }
    Util::EventBus& eventBus() { return eventBus_; }
    [[nodiscard]] Node* findNode(std::int64_t instanceId) const;

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
    const Lighting& getLighting() const {
        return this->lighting;
    }

protected:
    virtual void onReady() {}
    virtual void onUpdate(float dt) {}
};
