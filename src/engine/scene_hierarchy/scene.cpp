#include "scene.hpp"
#include "nodes/node.hpp"
#include "nodes/node3d.hpp"
#include "nodes/mesh_instance_3d.hpp"
#include "nodes/light_3d.hpp"
#include "scene_format.hpp"

Scene::Scene() : root(std::make_unique<Node>()) {
    root->name = "Root";
    root->scene = this;
}

Scene::~Scene() {
    deletionQueue.clear();
}

void Scene::SetRoot(std::unique_ptr<Node> node) {
    if (root) {
        root->OnExitTree();
    }
    root = std::move(node);
    if (root) {
        root->parent = nullptr;
        root->scene = this;
        root->OnCreate();
        root->OnEnterTree();
    }
}

void Scene::Update(float dt) {
    if (!root) return;

    PropagateReady(root.get());
    PropagateUpdate(root.get(), dt);
    ProcessQueuedDeletions();
}

void Scene::QueueNodeForDeletion(Node* node) {
    deletionQueue.push_back(node);
}

void Scene::ProcessQueuedDeletions() {
    std::vector<Node*> queue = std::move(deletionQueue);
    deletionQueue.clear();

    for (Node* node : queue) {
        if (!node) continue;
        if (node == root.get()) {
            root.reset();
            break;
        }
        if (node->parent) {
            std::unique_ptr<Node> removed = node->parent->RemoveChild(node);
        }
    }
}

void Scene::PropagateUpdate(Node* node, float dt) {
    node->Update(dt);
    for (auto& child : node->children) {
        PropagateUpdate(child.get(), dt);
    }
}

void Scene::PropagateReady(Node* node) {
    if (!node->ready_) {
        node->OnReady();
        node->ready_ = true;
    }
    for (auto& child : node->children) {
        PropagateReady(child.get());
    }
}

std::vector<Node3D*> Scene::GetAllNodes3D() {
    std::vector<Node3D*> result;
    if (root) CollectNodesOfType(root.get(), result);
    return result;
}

std::vector<MeshInstance3D*> Scene::GetAllMeshInstances() {
    std::vector<MeshInstance3D*> result;
    if (root) CollectMeshInstances(root.get(), result);
    return result;
}

std::vector<Light3D*> Scene::GetAllLights() {
    std::vector<Light3D*> result;
    if (root) CollectLights(root.get(), result);
    return result;
}

MeshInstance3D* Scene::GetSelectedMeshInstance() {
    for (auto* mi : GetAllMeshInstances()) {
        if (mi->selected) return mi;
    }
    return nullptr;
}

void Scene::CollectNodesOfType(Node* node, std::vector<Node3D*>& out) {
    if (auto* n3d = dynamic_cast<Node3D*>(node)) {
        out.push_back(n3d);
    }
    for (auto& child : node->children) {
        CollectNodesOfType(child.get(), out);
    }
}

void Scene::CollectMeshInstances(Node* node, std::vector<MeshInstance3D*>& out) {
    if (auto* mi = dynamic_cast<MeshInstance3D*>(node)) {
        out.push_back(mi);
    }
    for (auto& child : node->children) {
        CollectMeshInstances(child.get(), out);
    }
}

void Scene::CollectLights(Node* node, std::vector<Light3D*>& out) {
    if (auto* l = dynamic_cast<Light3D*>(node)) {
        out.push_back(l);
    }
    for (auto& child : node->children) {
        CollectLights(child.get(), out);
    }
}

bool Scene::Save(const std::string& filePath) {
    return SceneFormat::Save(*this, filePath);
}

bool Scene::Load(const std::string& filePath) {
    auto newScene = SceneFormat::Load(filePath);
    if (!newScene) return false;
    if (newScene->root) {
        SetRoot(std::move(newScene->root));
    }
    return true;
}

std::unique_ptr<Scene> Scene::CreateEmpty() {
    return std::make_unique<Scene>();
}

std::unique_ptr<Scene> Scene::LoadFromFile(const std::string& filePath) {
    return SceneFormat::Load(filePath);
}
