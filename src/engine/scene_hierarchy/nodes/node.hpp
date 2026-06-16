#pragma once

#include <string>
#include <vector>
#include <memory>

class Scene;

class Node {
public:
    std::string name;
    Node* parent = nullptr;
    std::vector<std::unique_ptr<Node>> children;

    virtual ~Node();

    virtual void OnCreate() {}
    virtual void OnDestroy() {}
    virtual void OnReady() {}
    virtual void OnEnterTree() {}
    virtual void OnExitTree() {}
    virtual void Update(float dt) {}

    void AddChild(std::unique_ptr<Node> child);
    std::unique_ptr<Node> RemoveChild(Node* child);
    Node* GetChild(const std::string& name);

    template<typename T>
    T* GetChildOfType() {
        for (auto& child : children) {
            T* casted = dynamic_cast<T*>(child.get());
            if (casted) return casted;
        }
        return nullptr;
    }

    std::vector<Node*> GetChildren();

    Node* GetParent() const { return parent; }

    Scene* GetScene() { return scene; }

    void QueueFree();

    virtual void BuildUI() {}

    virtual std::string GetNodeType() const { return "Node"; }

protected:
    Scene* scene = nullptr;
    bool queuedForDeletion = false;
    bool ready_ = false;

    friend class Scene;

private:
    static void SetSceneRecursive(Node* node, Scene* newScene);
    static void PropagateEnterTree(Node* node);
    static void PropagateExitTree(Node* node);
};
