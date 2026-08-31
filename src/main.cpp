#include <memory>

#include "core/window/window.hpp"
#include "core/scene/scene_manager.hpp"
#include "core/scene/editor.hpp"
#include "scene/basic_scene.hpp"
#include "core/renderer/shader_program.hpp"
#include "core/util/util.hpp"
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

    // Setup editor
    SceneManager sceneManager(window, meshRenderer);
    InputSystem input(window.getHandle());
    currentEnvironment = std::make_unique<Editor>(
        sceneManager,
        window,
        input
    );

    // Shader program and shader creation
    ShaderProgram program;
    Shader frag {"assets/shaders/3d_scene.frag", ShaderType::Fragment};
    Shader vert{"assets/shaders/3d_scene.vert", ShaderType::Vertex};

    // Need to compile the shaders before linking
    frag.compile();
    vert.compile();

    if (frag.getID() == 0 || vert.getID() == 0) {
        Util::Log::error("Failed to compile shaders");
        return 1;
    }
        
    // Attach and link programs
    program.attach(vert);
    program.attach(frag);
    program.link();

    // Init scene
    std::unique_ptr<Scene> scene = std::make_unique<BasicScene>();

    // Make nodes
    auto root = std::make_unique<Node3D>();
    auto mesh = std::make_unique<MeshInstance3D>();
    auto light = std::make_unique<Light3D>();

    light->setLightPosition({0.0, 0.0, 0.0});
    light->setEnergy(5.0f);
    light->setLightType(LightType::Directional);
    light->setLightDirection({70.0f, 0.0f, 0.0f});

    mesh->setName("studanka");

    // Load mesh
    ObjLoader loader;
    MeshData meshData = loader.Load(
        R"(C:\Users\lipov\Downloads\Studanka2\Studanka2.obj)",
        R"(C:\Users\lipov\Downloads\Studanka2\Studanka2.mtl)"
    );

    // Register mesh (better for sharing the same meshes)
    MeshResult res = scene->getMeshManager().add(meshData);

    // ! Test for rendering and instancing
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
    //     root->addChild(std::move(voxel));
    // }

    // Setup mesh and add node to tree
    mesh->setMesh(res.id, res.aabb);
    root->addChild(std::move(light));
    root->addChild(std::move(mesh));

    // Set the tree root
    scene->setRoot(std::move(root));

    // Test script attachment
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

        currentEnvironment->update(dt);

        // Clear screen from previous frame
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Frustum camFrustum = createFrustumFromCamera(currentEnvironment->getCamera(), window.getAspect());

        // Render
        RenderContext ctx {
            .program = program,
            .camera = currentEnvironment->getCamera(),
            .window = window,
            .camFrustum = camFrustum,
        };
        currentEnvironment->render(ctx);

        // Display frame
        window.present();
    }

    return 0;
}
