# Resource manager

Each `Scene` owns a `ResourceManager`. Resources are loaded into its cache and
identified by a `ResourceID`; nodes and render data keep the ID, and rendering
resolves it through the same scene manager.

Resource loading uses a type-specific `ResourceLoadInfo`. Loading returns an
empty `std::shared_ptr` when the resource rejects its load info, and the manager
logs the failure.

```cpp
MeshResourceLoadInfo loadInfo;
loadInfo.objectFile = "meshes/cube.obj";
loadInfo.materialFile = "meshes/cube.mtl";

auto resource = scene.getResourceManager().load<MeshResource>(loadInfo);
if (!resource)
    return;

meshInstance.setMesh(resource->getID());
```

`MeshResourceLoadInfo` can also receive existing CPU `MeshData` for procedurally
generated meshes. `getResource<T>(id)` returns an empty pointer when the ID is
not cached or does not identify a `T`. `unload(id)` removes the resource from
the cache; IDs are not reused.
