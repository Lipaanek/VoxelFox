# ECS → Node System Redesign Plan

## Overview
Replace the partially-implemented ECS (`ComponentStorage<T>`, `System`, `Entity`) with a Godot-style **Node tree hierarchy**. Nodes are the only game objects — they have lifecycle callbacks, properties, methods, and form a parent-child scene tree.

---

## Design Decisions

| Decision | Choice |
|----------|--------|
| ECS vs Pure Node | **Pure Node hierarchy** — no entity/component separation |
| Lifecycle callbacks | `OnCreate` / `OnDestroy`, `OnEnterTree` / `OnExitTree`, `OnReady`, `Update(dt)` |
| Rendering model | `MeshInstance3D` nodes, scene tree traversal |
| Built-in types | `Node3D`, `MeshInstance3D`, `Light3D` |
| Scene file format | Custom text format (`.vsf`) |
| Scripting | C++ inheritance now, Lua support designed for later |
| Property access style | Public fields + convenience methods |
| MeshManager | Keep as asset store — nodes reference assets by handle |

---

## Phase 1: Core Node System

### New Files

| File | Purpose |
|------|---------|
| `src/engine/ecs/node.hpp` | **Rewritten** — base `Node` class with lifecycle, tree ops, `QueueFree()`, `BuildUI()` |
| `src/engine/ecs/node3d.hpp` | **New** — `Node3D : Node` with `position`, `eulerRotation`, `scale`, `GetGlobalTransform()`, `LookAt()` |
| `src/engine/ecs/mesh_instance_3d.hpp` | **New** — `MeshInstance3D : Node3D` holds mesh asset path into `MeshManager`, selection state, bbox access |
| `src/engine/ecs/light_3d.hpp` | **New** — `Light3D : Node3D` with `color`, `intensity` |
| `src/engine/ecs/scene.hpp` | **New** — `Scene` owns root `Node`, manages update traversal, deferred destruction, save/load |
| `src/engine/ecs/scene_format.hpp` | **New** — Custom text format (.vsf) serializer/deserializer |
| `src/engine/ecs/scene_format.cpp` | **New** — Implementation |

### Files to Remove

| File | Reason |
|------|--------|
| `src/engine/ecs/ecs.hpp` | `ComponentStorage`, `SystemManager`, `ECS` class replaced by Node system |
| `src/engine/ecs/components.hpp` | `Transform` struct replaced by `Node3D` properties |
| `src/engine/ecs/system.hpp` | `System` abstract base replaced by `Node::Update()` |
| `src/engine/ecs/property_system.hpp` | Replaced by `Node::BuildUI()` virtual method |
| `src/engine/ecs/nodes/model_3d.hpp` | Broken `Model3D` replaced by `MeshInstance3D` |

---

## Key Node API Design

### Base Node (`node.hpp`)

```cpp
class Node {
public:
    std::string name;
    Node* parent = nullptr;
    std::vector<std::unique_ptr<Node>> children;

    // Lifecycle
    virtual ~Node();
    virtual void OnCreate() {}
    virtual void OnDestroy() {}
    virtual void OnReady() {}          // Called once after entire tree is ready
    virtual void OnEnterTree() {}      // When added to scene
    virtual void OnExitTree() {}       // When removed from scene
    virtual void Update(float dt) {}

    // Tree operations
    void AddChild(std::unique_ptr<Node> child);
    std::unique_ptr<Node> RemoveChild(Node* child);
    Node* GetChild(const std::string& name);
    template<typename T> T* GetChildOfType();
    std::vector<Node*> GetChildren();
    Node* GetParent() const { return parent; }

    // Scene ownership
    Scene* GetScene() { return scene; }

    // Deferred destruction
    void QueueFree();

    // Editor UI — each node draws its own property panel
    virtual void BuildUI() {}

protected:
    Scene* scene = nullptr;
    bool queuedForDeletion = false;

    friend class Scene;
};
```

### Node3D (`node3d.hpp`)

`eulerRotation` is the editor-facing field (stored as vec3 in degrees). `GetQuaternion()` converts to quat for engine math. `GetLocalTransform()` builds the matrix from scale → quat(euler) → translation.

```cpp
class Node3D : public Node {
public:
    glm::vec3 position{0.0f};
    glm::vec3 eulerRotation{0.0f};   // Editor-facing Euler angles (degrees)
    glm::vec3 scale{1.0f};

    // Convenience methods
    void SetPosition(const glm::vec3& pos);
    void Translate(const glm::vec3& delta);
    void SetScale(const glm::vec3& scl);
    void Rotate(const glm::vec3& axis, float angle);
    void LookAt(const glm::vec3& target);

    // Quaternion conversion
    glm::quat GetQuaternion() const;                  // eulerRotation → quat
    void SetQuaternion(const glm::quat& q);           // quat → eulerRotation

    // Transform matrices
    glm::mat4 GetLocalTransform() const;              // TRS from position, eulerRotation, scale
    glm::mat4 GetGlobalTransform() const;             // Composes with parent chain

    // Editor UI
    void BuildUI() override;

private:
    bool transformDirty = true;
    glm::mat4 localTransform{1.0f};
    void UpdateTransform();
};
```

### MeshInstance3D (`mesh_instance_3d.hpp`)

```cpp
class MeshInstance3D : public Node3D {
public:
    std::string meshAssetPath;       // Relative path into MeshManager
    bool selected = false;

    // Voxel / bbox access via MeshManager lookup
    glm::vec3 GetBBoxMin() const;
    glm::vec3 GetBBoxMax() const;
    const std::vector<VoxelChunk>* GetVoxelChunks() const;
    Mesh* GetRenderMesh() const;

    void SetMesh(const std::string& assetPath);
    const std::string& GetMesh() const { return meshAssetPath; }

    // Editor UI
    void BuildUI() override;
};
```

### Light3D (`light_3d.hpp`)

Simple point light matching current shader uniforms (`lightPos`, `lightColor`).

```cpp
class Light3D : public Node3D {
public:
    glm::vec3 color{1.0f, 1.0f, 1.0f};
    float intensity = 1.0f;

    // Editor UI
    void BuildUI() override;
};
```

---

## Scene Management (`scene.hpp`)

```cpp
class Scene {
public:
    Scene();
    ~Scene();

    // Root node access
    Node* GetRoot() const { return root.get(); }
    void SetRoot(std::unique_ptr<Node> node);

    // Lifecycle — called each frame
    void Update(float dt);
    void ProcessQueuedDeletions();

    // Scene traversal helpers
    std::vector<Node3D*> GetAllNodes3D();
    std::vector<MeshInstance3D*> GetAllMeshInstances();
    std::vector<Light3D*> GetAllLights();
    MeshInstance3D* GetSelectedMeshInstance();

    // Serialization
    bool Save(const std::string& filePath);
    bool Load(const std::string& filePath);

    static std::unique_ptr<Scene> CreateEmpty();
    static std::unique_ptr<Scene> LoadFromFile(const std::string& filePath);

private:
    std::unique_ptr<Node> root;
    std::vector<Node*> deletionQueue;

    void PropagateUpdate(Node* node, float dt);
    void PropagateReady(Node* node);
};
```

---

## Scene File Format (`.vsf`)

Custom text format — human-readable, indentation-based tree structure:

```
scene "Untitled Scene"
{
    node "MeshInstance3D" "MyMesh"
    {
        property "position" = (0, 0, 0)
        property "eulerRotation" = (0, 0, 0)
        property "scale" = (1, 1, 1)
        property "mesh" = "assets/objects/my_mesh.voxf"

        node "MeshInstance3D" "ChildMesh"
        {
            property "position" = (5, 0, 0)
        }
    }

    node "Light3D" "Sun"
    {
        property "position" = (10, 20, 10)
        property "color" = (1, 1, 1)
        property "intensity" = 1.0
    }
}
```

### Property Serialization Mapping

| C++ Type | Text Format |
|----------|-------------|
| `float` | `1.0` |
| `glm::vec3` | `(x, y, z)` |
| `std::string` | `"value"` |
| `bool` | `true` / `false` |

---

## Phase 2: Integration into Engine Runtime

### Files to Modify

| File | Changes |
|------|---------|
| `src/main.cpp` | Create `Scene` instance, call `scene.Update(dt)` in main loop, pass scene to editor/playtest screens |
| `src/engine/gui/screens/main_editor_screen.hpp` | Replace `MeshManager&` with `Scene&`, add scene rendering methods |
| `src/engine/gui/screens/main_editor_screen.cpp` | Replace `MeshManager` iteration with scene tree traversal. Drag-drop creates `MeshInstance3D` nodes. Property panel calls `selectedNode->BuildUI()`. Click selection queries scene tree via ray-AABB on all `MeshInstance3D` nodes. |
| `src/engine/gui/screens/playtest_screen.hpp` | Replace `MeshManager&` with `Scene&` |
| `src/engine/gui/screens/playtest_screen.cpp` | Traverse scene tree for rendering instead of `MeshManager` iteration |

### Files Kept As-Is

| File | Reason |
|------|--------|
| `src/engine/mesh_manager.hpp` / `.cpp` | Pure asset store — no scene logic |

---

## Phase 3: Editor Property Panel

Editor property panel calls `selectedNode->BuildUI()` on the currently selected node. Each node type draws its own ImGui controls:

- **Node3D::BuildUI()** — draws `position` (DragFloat3), `eulerRotation` (DragFloat3), `scale` (DragFloat3)
- **MeshInstance3D::BuildUI()** — calls `Node3D::BuildUI()`, then draws mesh asset path, voxel info
- **Light3D::BuildUI()** — calls `Node3D::BuildUI()`, then draws `color` (ColorEdit3), `intensity` (DragFloat)

This replaces the hardcoded Position DragFloat3 in the current editor panel. No `PropertyDescriptor` infrastructure needed.

---

## Implementation Order

1. **Remove** old ECS files (`ecs.hpp`, `components.hpp`, `system.hpp`, `property_system.hpp`, `nodes/model_3d.hpp`)
2. **Write** `scene.hpp` + `scene.cpp` (scene lifecycle, update propagation, deferred deletion)
3. **Write** `node.hpp` (base Node with tree ops and lifecycle)
4. **Write** `node3d.hpp` (spatial transform with global/local matrix)
5. **Write** `mesh_instance_3d.hpp` (mesh handle, selection, bbox)
6. **Write** `light_3d.hpp` (light properties)
7. **Write** `scene_format.hpp` + `scene_format.cpp` (`.vsf` parser/serializer)
8. **Modify** `main.cpp` to create and drive a Scene
9. **Modify** `main_editor_screen.hpp/.cpp` to use scene tree
10. **Modify** `playtest_screen.hpp/.cpp` to use scene tree
11. **Build and test**
