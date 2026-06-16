#pragma once

#include <memory>
#include <vector>
#include <string>

class Node;
class Node3D;
class MeshInstance3D;
class Light3D;

class Scene {
public:
    Scene();
    ~Scene();

    Node* GetRoot() const { return root.get(); }
    void SetRoot(std::unique_ptr<Node> node);

    void Update(float dt);
    void ProcessQueuedDeletions();
    void QueueNodeForDeletion(Node* node);

    std::vector<Node3D*> GetAllNodes3D();
    std::vector<MeshInstance3D*> GetAllMeshInstances();
    std::vector<Light3D*> GetAllLights();
    MeshInstance3D* GetSelectedMeshInstance();

    bool Save(const std::string& filePath);
    bool Load(const std::string& filePath);

    static std::unique_ptr<Scene> CreateEmpty();
    static std::unique_ptr<Scene> LoadFromFile(const std::string& filePath);

private:
    std::unique_ptr<Node> root;
    std::vector<Node*> deletionQueue;

    void PropagateUpdate(Node* node, float dt);
    void PropagateReady(Node* node);
    void CollectNodesOfType(Node* node, std::vector<Node3D*>& out);
    void CollectMeshInstances(Node* node, std::vector<MeshInstance3D*>& out);
    void CollectLights(Node* node, std::vector<Light3D*>& out);
};
