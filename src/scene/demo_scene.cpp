#include "demo_scene.hpp"

#include "core/scene/scene.hpp"
#include "core/model_loading/obj_loader.hpp"
#include "core/resources/resource_types/mesh_resource.hpp"
#include "core/util/util.hpp"
#include "nodes/light_3d.hpp"
#include "nodes/mesh_instance_3d.hpp"
#include "nodes/node3d.hpp"
#include "nodes/voxel_model.hpp"

std::unique_ptr<Scene> createDemoScene() {
    auto scene = std::make_unique<Scene>();

    ObjLoader loader;

    const MeshData wellMeshData = loader.load(
        R"(C:\Users\lipov\Downloads\Studanka2\Studanka2.obj)",
        R"(C:\Users\lipov\Downloads\Studanka2\Studanka2.mtl)"
    );

    if (wellMeshData.vertices.empty() || wellMeshData.indices.empty()) {
        Util::Log::error("Failed to load demo mesh");
        return {};
    }

    MeshResourceLoadInfo meshLoadInfo;
    meshLoadInfo.meshData = &wellMeshData;

    const auto meshResource = scene->getResourceManager().load<MeshResource>(meshLoadInfo);

    if (!meshResource)
        return {};

    auto root = std::make_unique<Node3D>();
    auto mesh = std::make_unique<MeshInstance3D>();
    auto light = std::make_unique<Light3D>();

    light->setLightPosition({0.0f, 0.0f, 0.0f});
    light->setEnergy(1.0f);
    light->setLightType(LightType::Directional);
    light->setLightDirection({1.0f, -1.0f, -1.0f});

    mesh->setName("studanka");
    mesh->setMesh(meshResource->getID());

    auto voxelModel = std::make_unique<VoxelModel>("Model", 0.1f);
    voxelModel->voxelize(wellMeshData);
    voxelModel->setPosition({5.0f, 0.0f, 0.0f});

    root->addChild(std::move(light));
    root->addChild(std::move(mesh));
    root->addChild(std::move(voxelModel));
    scene->setRoot(std::move(root));

    return scene;
}

void attachDemoSceneScript(Scene& scene) {
    Node* meshNode = scene.getRoot()->getChild("studanka");
    if (!meshNode)
        return;

    const auto scriptResult = meshNode->setScript(
        "assets/scripts/test_node_script.lua",
        {}
    );
    Util::Log::scriptLoadLog(scriptResult);
}
