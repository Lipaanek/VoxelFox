#pragma once

#include <memory>
#include <vector>
#include "../../nodes/node.hpp"
#include "../lighting/lighting.hpp"
#include "core/gui/ui_element.hpp"
#include "core/resources/resource_manager.hpp"
#include "core/util/util.hpp"
#include "core/util/event_bus.hpp"

class Scene {
private:
    Util::EventBus eventBus_;
    LuaEngine lua_;
    std::unique_ptr<Node> root = nullptr;
    Lighting lighting;
    std::vector<UIElement> uiElements;
    ResourceManager resourceManager;

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

    void addUIElement(UIElement element);

    [[nodiscard]] Node* findNode(std::int64_t instanceId) const;
    [[nodiscard]] std::vector<UIElement>& getUIElements();
    [[nodiscard]] const std::vector<UIElement>& getUIElements() const;

    virtual void update(const float dt) {
        onUpdate(dt);
        lua_.runUpdate(dt);
    }

    void ready() {
        Util::Log::log("Ready scene");
        onReady();
        lua_.runReady();
    }

    Lighting& getLighting() {
        return this->lighting;
    }

    ResourceManager& getResourceManager() {
        return this->resourceManager;
    }

    const ResourceManager& getResourceManager() const {
        return this->resourceManager;
    }

    const Lighting& getLighting() const {
        return this->lighting;
    }

protected:
    virtual void onReady() {}
    virtual void onUpdate(float dt) {}
};
