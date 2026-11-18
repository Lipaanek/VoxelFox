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
    Util::EventBus m_eventBus_;
    LuaEngine m_lua_;
    std::unique_ptr<Node> m_root = nullptr;
    Lighting m_lighting;
    std::vector<UIElement> m_uiElements;
    ResourceManager m_resourceManager;

public:
    Scene();
    virtual ~Scene();

    void setRoot(std::unique_ptr<Node> newRoot) {
        m_root = std::move(newRoot);

        if (m_root) {
            m_root->setScene(this);
        }
    }

    Node* getRoot() const { return this->m_root.get(); }
    LuaEngine& lua() { return m_lua_; }
    Util::EventBus& eventBus() { return m_eventBus_; }

    void addUIElement(UIElement element);

    [[nodiscard]] Node* findNode(std::int64_t instanceId) const;
    [[nodiscard]] std::vector<UIElement>& getUIElements();
    [[nodiscard]] const std::vector<UIElement>& getUIElements() const;

    virtual void update(const float dt) {
        onUpdate(dt);
        m_lua_.runUpdate(dt);
    }

    void ready() {
        Util::Log::log("Ready scene");
        onReady();
        m_lua_.runReady();
    }

    Lighting& getLighting() {
        return this->m_lighting;
    }

    ResourceManager& getResourceManager() {
        return this->m_resourceManager;
    }

    const ResourceManager& getResourceManager() const {
        return this->m_resourceManager;
    }

    const Lighting& getLighting() const {
        return this->m_lighting;
    }

protected:
    virtual void onReady() {}
    virtual void onUpdate(float dt) {}
};
