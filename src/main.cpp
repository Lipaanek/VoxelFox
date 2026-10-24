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
#include "nodes/voxel_model.hpp"
#include "core/gui/ui_builder.hpp"

int main() {
    WindowSettings settings {
        .width = 1920,
        .height = 1080,
        .title = "VoxelFox",
        .vsync = VSync::VSyncEnabled
    };

    Window window(settings);

    Camera camera;

    Renderer renderer;

    // Setup editor
    SceneManager sceneManager(window, renderer, camera);
    InputSystem input(window);
    std::unique_ptr<Environment> currentEnvironment = std::make_unique<Editor>(
        sceneManager,
        window,
        input,
        camera
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
    light->setEnergy(1.0f);
    light->setLightType(LightType::Directional);
    light->setLightDirection({1.0f, -1.0f, -1.0f});

    mesh->setName("studanka");

    // Load mesh
    ObjLoader loader;
    MeshData wellMeshData = loader.Load(
        R"(C:\Users\lipov\Downloads\Studanka2\Studanka2.obj)",
        R"(C:\Users\lipov\Downloads\Studanka2\Studanka2.mtl)"
    );

    const MeshData& preVoxelwellMesh = wellMeshData;

    auto voxelModel = std::make_unique<VoxelModel>("Model", 0.1f);
    voxelModel->voxelize(preVoxelwellMesh);

    voxelModel->setPosition({ 5.0f, 0.0f, 0.0f });

    // Register mesh (better for sharing the same meshes)
    MeshResult res = scene->getMeshManager().add(wellMeshData);

    // Setup mesh and add node to tree
    mesh->setMesh(res.id);
    root->addChild(std::move(light));
    root->addChild(std::move(mesh));
    root->addChild(std::move(voxelModel));

    // Set the tree root
    scene->setRoot(std::move(root));

    auto* wellMesh = scene->getRoot()->getChild("studanka");
    currentEnvironment->setScene(std::move(scene));

    // Script attachment
    if (wellMesh) {
        auto scriptRes = wellMesh->setScript(
            "assets/scripts/test_node_script.lua",
            {}
        );
        Util::Log::scriptLoadLog(scriptRes);
    }

    currentEnvironment->addUIElement(makeWindow(
        "main_window",
        "VoxelFox GUI Test Window",{
            makeText("welcome", "Welcome!"),
            makeButton("action", "Click me", [] {
                Util::Log::log("UI button was clicked.");
            }),
            makeCheckbox("enabled", "Enabled", [] {
                Util::Log::log("UI checkbox was clicked.");
            }),
        }
    ));

    double lastTime = glfwGetTime();
    while (!window.shouldClose()) {
        const double currentTime = glfwGetTime();
        const auto dt = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;

        input.beginInput();
        window.update();

        currentEnvironment->beginUI();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render world
        currentEnvironment->update(dt);
        currentEnvironment->render();

        // Display frame
        window.present();
    }

    return 0;
}
