#include <memory>

#include "core/window/window.hpp"
#include "core/scene/scene_manager.hpp"
#include "core/scene/editor.hpp"
#include "scene/basic_scene.hpp"
#include "core/renderer/shader_program.hpp"
#include "core/util/util.hpp"
#include "core/util/chunk_manager.hpp"
#include "core/input/input_system.hpp"
#include "core/model_loading/obj_loader.hpp"
#include "core/scripting/lua_engine.hpp"
#include "nodes/light_3d.hpp"
#include "nodes/mesh_instance_3d.hpp"
#include "nodes/node3d.hpp"
#include "nodes/voxel.hpp"

std::unique_ptr<Environment> currentEnvironment;

int main() {
    Window window("VoxelFox", 1920, 1080);

    MeshRenderer meshRenderer;

    SceneManager sceneManager(window, meshRenderer);
    InputSystem input(window.getHandle());
    currentEnvironment = std::make_unique<Editor>(
        sceneManager,
        window,
        input
    );

    ShaderProgram program;
    Shader frag {"assets/shaders/3d_scene.frag", ShaderType::Fragment};
    Shader vert{"assets/shaders/3d_scene.vert", ShaderType::Vertex};

    frag.compile();
    vert.compile();

    if (frag.getID() == 0 || vert.getID() == 0) {
        Util::Log::error("Failed to compile shaders");
        return 1;
    }

    program.attach(vert);
    program.attach(frag);
    program.link();

    std::unique_ptr<Scene> scene = std::make_unique<BasicScene>();

    auto mesh = std::make_unique<MeshInstance3D>();
    auto light = std::make_unique<Light3D>();

    light->setLightPosition({0.0, 0.0, 0.0});
    light->setEnergy(5.0f);
    light->setLightType(LightType::Directional);
    light->setLightDirection({70.0f, 0.0f, 0.0f});

    mesh->setName("studanka");

    ObjLoader loader;
    MeshData meshData = loader.Load(
        R"(C:\Users\lipov\Downloads\Studanka2\Studanka2.obj)",
        R"(C:\Users\lipov\Downloads\Studanka2\Studanka2.mtl)"
    );

    MeshResult res = scene->getMeshManager().add(meshData);

    scene->setRoot(std::make_unique<Node3D>());

    // constexpr int voxelCount = 10'000;
    // constexpr float voxelSize = 1.0f;
    // constexpr int gridSize = 22;
    //
    // for (int i = 0; i < voxelCount; ++i) {
    //     auto voxel = std::make_unique<Voxel>();
    //
    //     int x = i % gridSize;
    //     int y = (i / gridSize) % gridSize;
    //     int z = i / (gridSize * gridSize);
    //
    //     voxel->setSize(voxelSize);
    //     voxel->setPosition({
    //         static_cast<float>(x),
    //         static_cast<float>(y),
    //         static_cast<float>(z)
    //     });
    //
    //     MeshInstance3D* voxelPtr = voxel.get();
    //     scene->getRoot()->addChild(std::move(voxel));
    //     scene->getChunkManager().registerNode(voxelPtr);
    // }

    mesh->setMesh(res.id, res.aabb);
    scene->getRoot()->addChild(std::move(light));
    scene->getRoot()->addChild(std::move(mesh));

    if (auto* wellMesh = scene->getRoot()->getChild("studanka")) {
        auto scriptRes = wellMesh->setScript(
            "assets/scripts/test_node_script.lua",
            {}
        );
        Util::Log::scriptLoadLog(scriptRes);
    }

    currentEnvironment->setScene(std::move(scene));

    double lastTime = glfwGetTime();
    while (!window.shouldClose()) {
        input.beginInput();
        window.update();

        double now = glfwGetTime();
        auto dt = static_cast<float>(now - lastTime);
        lastTime = now;

        //Util::Log::log(std::to_string(dt));

        currentEnvironment->update(dt);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Frustum camFrustum = createFrustumFromCamera(currentEnvironment->getCamera(), window.getAspect());

        RenderContext ctx {
            .program = program,
            .camera = currentEnvironment->getCamera(),
            .window = window,
            .camFrustum = camFrustum,
        };
        currentEnvironment->render(ctx);

        window.present();
    }

    return 0;
}
